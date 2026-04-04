/**
 * @file take_screenshot_action.cpp
 * @brief Implementation of the visual verification tool.
 *
 * Interfaces with the host os to capture the primary screen, optionally crops 
 * to the active background process window, prompts the user for security 
 * clearance, and formats the specialized html response for the main window.
 */

#include "take_screenshot_action.h"
#include "execute_shell_action.h"

#include <QGuiApplication>
#include <QScreen>
#include <QPixmap>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QWidget>
#include <QDateTime>

// include windows api for precise window cropping during screenshots
#ifdef Q_OS_WIN
#include <windows.h>
#include <tlhelp32.h>
#include <vector>

struct WindowSearchData {
    std::vector<DWORD> pids;
    HWND hwnd;
    long maxArea;
};

// helper function to recursively find all child processes of the shell (e.g., cmd.exe -> python.exe)
void findChildPids(DWORD parentPid, std::vector<DWORD>& pids) {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hSnap, &pe)) {
            do {
                if (pe.th32ParentProcessID == parentPid) {
                    pids.push_back(pe.th32ProcessID);
                    findChildPids(pe.th32ProcessID, pids); // recurse just in case
                }
            } while (Process32Next(hSnap, &pe));
        }
        CloseHandle(hSnap);
    }
}

// callback to locate the specific os window owned by the agent's active process tree
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    WindowSearchData* data = reinterpret_cast<WindowSearchData*>(lParam);
    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);
    
    // check if this window belongs to our shell or any of its child scripts
    bool isTargetPid = false;
    for (DWORD pid : data->pids) {
        if (processId == pid) { 
            isTargetPid = true; 
            break; 
        }
    }
    
    if (isTargetPid && IsWindowVisible(hwnd)) {
        RECT rect;
        if (GetWindowRect(hwnd, &rect)) {
            long area = (rect.right - rect.left) * (rect.bottom - rect.top);
            
            // prioritize the largest visible window (avoids grabbing invisible 1x1 tool stubs)
            if (area > data->maxArea) {
                data->hwnd = hwnd;
                data->maxArea = area;
            }
        }
    }
    // keep searching to ensure we get the primary gui, not a console wrapper
    return TRUE;
}
#endif

// ============================================================================
// constructor
// ============================================================================

TakeScreenshotAction::TakeScreenshotAction(ExecuteShellAction* shellAction, QObject* parent) 
    : BaseAgentAction(parent), activeShell(shellAction) {}

// ============================================================================
// public interface
// ============================================================================

QString TakeScreenshotAction::getName() const {
    return "take_screenshot";
}

// ============================================================================
// execution logic
// ============================================================================

void TakeScreenshotAction::execute(const AgentCommand& command, const QString& workspacePath) {
    Q_UNUSED(command);

    // ask the shell action for the pid of the background gui app
    qint64 pid = activeShell->getActiveProcessId();
    if (pid == 0) {
        emit actionFinished("System Error [take_screenshot]: No active GUI application is currently running to screenshot.");
        return;
    }

    QScreen* screen = QGuiApplication::primaryScreen();
    if (!screen) {
        emit actionFinished("System Error [take_screenshot]: Could not access the primary display.");
        return;
    }

    // capture the entire primary display buffer
    QPixmap fullScreen = screen->grabWindow(0);
    QPixmap targetPixmap = fullScreen;

    // --- winapi cropper restoration ---
#ifdef Q_OS_WIN
    WindowSearchData searchData;
    searchData.hwnd = nullptr;
    searchData.maxArea = 0;
    searchData.pids.push_back(static_cast<DWORD>(pid));
    
    // populate the tree with all child pids so we catch python.exe, node.exe, etc.
    findChildPids(static_cast<DWORD>(pid), searchData.pids);

    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&searchData));

    if (searchData.hwnd) {
        RECT rect;
        if (GetWindowRect(searchData.hwnd, &rect)) {
            int x = rect.left;
            int y = rect.top;
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;
            
            // successfully cropped to the specific window!
            targetPixmap = fullScreen.copy(x, y, width, height); 
        }
    }
#endif

    // scale down the resolution to prevent massive base64 token payloads to google's api
    QPixmap scaledPixmap = targetPixmap.scaled(1280, 720, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // --- privacy intercept restoration ---
    
    // cast the parent object to a qwidget so the messagebox centers perfectly over the main ui
    QWidget* parentWidget = qobject_cast<QWidget*>(parent());
    
    QMessageBox imgBox(parentWidget);
    imgBox.setWindowTitle("Security Intercept: Approve Image");
    imgBox.setText("The agent wants to attach this screenshot. Do you approve?");
    
    // show a small preview in the dialog box
    imgBox.setIconPixmap(scaledPixmap.scaledToWidth(400, Qt::SmoothTransformation));
    imgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    
    if (imgBox.exec() == QMessageBox::Yes) {
        // write to the expected hardcoded path in the sandbox
        QString fileName = "latest_agent_screenshot.png";
        QString filePath = QDir(workspacePath).absoluteFilePath(fileName);

        if (scaledPixmap.save(filePath, "PNG")) {
            // append the current millisecond timestamp so the Qt TextBrowser is forced to reload the image
            qint64 cacheBuster = QDateTime::currentMSecsSinceEpoch();
            
            // format the specific string the main window looks for to intercept and attach the file
            QString uiMessage = QString("System [take_screenshot]: Visual verification captured.<br><br>"
                                        "<img src=\"file:///%1?v=%2\" width=\"400\" style=\"border-radius: 4px;\">")
                                        .arg(filePath).arg(cacheBuster);
            emit actionFinished(uiMessage);
        } else {
            emit actionFinished("System Error [take_screenshot]: Failed to save the image buffer to disk.");
        }
    } else {
        emit actionFinished("System Error [take_screenshot]: The human user DENIED the screenshot request due to privacy concerns.");
    }
}