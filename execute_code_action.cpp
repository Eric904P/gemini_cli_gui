#include "execute_code_action.h"
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QUuid>
#include <QTextStream>

ExecuteCodeAction::ExecuteCodeAction(QObject* parent) : BaseAgentAction(parent) {}

QString ExecuteCodeAction::getName() const {
    return "execute_code";
}

// Maps directly to your new Zero-Setup runtimes folder
QString ExecuteCodeAction::getBundledRuntimePath(const QString& language) const {
    QString appDir = QCoreApplication::applicationDirPath();
    
    if (language == "python") {
        return QDir(appDir).filePath("runtimes/python/python.exe");
    } else if (language == "javascript") {
        return QDir(appDir).filePath("runtimes/node/node.exe");
    } else if (language == "cpp") {
        return QDir(appDir).filePath("runtimes/mingw/bin/g++.exe");
    }
    
    return "";
}

void ExecuteCodeAction::execute(const AgentCommand& command, const QString& workspacePath) {
    QString language = command.target.toLower(); 
    QString code = command.payload;

    QString runtimePath = getBundledRuntimePath(language);
    if (!QFile::exists(runtimePath)) {
        emit actionFinished(QString("System Error: Bundled runtime for '%1' not found at %2").arg(language, runtimePath));
        return;
    }

    // Create a temporary file to hold the code
    QString fileName = "temp_" + QUuid::createUuid().toString(QUuid::WithoutBraces).left(8);
    QString filePath = QDir(workspacePath).filePath(fileName + (language == "python" ? ".py" : language == "javascript" ? ".js" : ".cpp"));

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit actionFinished("System Error: Could not write temporary code file.");
        return;
    }
    QTextStream out(&file);
    out << code;
    file.close();

    QString finalOutput;
    QProcess process;
    process.setWorkingDirectory(workspacePath);

    if (language == "cpp") {
        QString outExe = QDir(workspacePath).filePath(fileName + ".exe");
        
        // Step 1: Compile with MinGW
        process.start(runtimePath, QStringList() << filePath << "-o" << outExe);
        if (!process.waitForFinished(10000)) {
            process.kill();
            finalOutput = "System Error: Compilation timed out.";
        } else if (process.exitCode() != 0) {
            finalOutput = "Compilation Error:\n" + QString::fromUtf8(process.readAllStandardError());
        } else {
            // Step 2: Execute the compiled binary
            QProcess runProcess;
            runProcess.setWorkingDirectory(workspacePath);
            runProcess.start(outExe, QStringList());
            if (runProcess.waitForFinished(15000)) {
                finalOutput = QString::fromUtf8(runProcess.readAllStandardOutput()) + QString::fromUtf8(runProcess.readAllStandardError());
            } else {
                runProcess.kill();
                finalOutput = "System Error: Execution timed out (Possible infinite loop).";
            }
            QFile::remove(outExe); // Cleanup binary
        }
    } else {
        // Interpreted Execution (Python/Node)
        process.start(runtimePath, QStringList() << filePath);
        if (process.waitForFinished(15000)) {
            finalOutput = QString::fromUtf8(process.readAllStandardOutput()) + QString::fromUtf8(process.readAllStandardError());
        } else {
            process.kill();
            finalOutput = "System Error: Execution timed out (Possible infinite loop).";
        }
    }

    QFile::remove(filePath); // Cleanup source code
    
    if (finalOutput.trimmed().isEmpty()) finalOutput = "Success (No terminal output).";
    emit actionFinished(QString("System [execute_code %1]:\n%2").arg(language, finalOutput.trimmed()));
}