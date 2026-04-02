/**
 * @file take_screenshot_action.cpp
 * @brief Implementation of the screenshot tool.
 *
 * Uses QScreen and Windows API calls to capture application UI, prompts
 * the user for security clearance, and writes the image to the workspace.
 */

#include "take_screenshot_action.h"
#include "execute_shell_action.h"

#include <QGuiApplication>
#include <QScreen>
#include <QDir>
#include <QMessageBox>
#include <QPixmap>
#include <QByteArray>
#include <QBuffer>
#include <QUrl>

// include windows api for precise window cropping during screenshots
#ifdef Q_OS_WIN
#include <windows.h>

struct WindowSearchData {
    DWORD pid;
    HWND hwnd;
};

// callback to locate the specific os window owned by the agent's active process
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    WindowSearchData* data = reinterpret_cast<WindowSearchData*>(lParam);
    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);
    
    // stop searching if we find a visible window belonging to the exact pid
    if (processId == data->pid && IsWindowVisible(hwnd)) {
        data->hwnd = hwnd;
        return FALSE; 
    }
    return TRUE;
}
#endif

TakeScreenshotAction::TakeScreenshotAction(ExecuteShellAction* shellAction, QObject* parent) 
    : BaseAgentAction(parent), linkedShellAction(shellAction) {}

QString TakeScreenshotAction::getName() const {
    return "take_screenshot";
}

QPixmap TakeScreenshotAction::captureProcessWindow(qint64 processId) {
    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen) return QPixmap();

    QPixmap fullScreen = screen->grabWindow(0);

#ifdef Q_OS_WIN
    WindowSearchData searchData = { static_cast<DWORD>(processId), nullptr };
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&searchData));

    if (searchData.hwnd) {
        RECT rect;
        if (GetWindowRect(searchData.hwnd, &rect)) {
            int x = rect.left;
            int y = rect.top;
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;
            
            return fullScreen.copy(x, y, width, height);
        }
    }
#endif

    return fullScreen; 
}

void TakeScreenshotAction::execute(const AgentCommand& command, const QString& workspacePath) {
    qint64 pid = linkedShellAction->getActiveProcessId();
    
    if (pid == 0) {
        emit actionFinished("[system error: no active gui application is currently running to screenshot.]");
        return;
    }

    QPixmap croppedShot = captureProcessWindow(pid);
    
    if (croppedShot.isNull()) {
         emit actionFinished("[system error: failed to capture window. it may be minimized or hidden.]");
         return;
    }
    
    QMessageBox imgBox;
    imgBox.setWindowTitle("Security Intercept: Approve Image");
    imgBox.setText("The agent wants to read this image. Do you approve?");
    imgBox.setIconPixmap(croppedShot.scaledToWidth(400, Qt::SmoothTransformation));
    imgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    
    if (imgBox.exec() == QMessageBox::Yes) {
        // 1. Save to disk with a predictable name for the attachment pipeline
        QString savePath = QDir(workspacePath).absoluteFilePath("latest_agent_screenshot.png");
        croppedShot.save(savePath, "PNG");

        // 2. Format a lightweight HTML tag using the local file path (NO massive Base64 string!)
        QString fileUrl = QUrl::fromLocalFile(savePath).toString();
        QString htmlImage = QString(
            "<br><br>"
            "<img src=\"%1\" width=\"100%\" "
            "style=\"border: 1px solid #475569; border-radius: 4px;\" />"
        ).arg(fileUrl);

        emit actionFinished(QString("[system take_screenshot]: Visual verification captured.%1").arg(htmlImage));
    } else {
        emit actionFinished("[system error: the human user DENIED the screenshot request due to privacy concerns.]");
    }
}