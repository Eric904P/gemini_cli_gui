/**
 * @file git_manager_action.cpp
 * @brief Implementation of the Git Manager Action.
 */

#include "git_manager_action.h"
#include <QProcess>
#include <QJsonDocument>
#include <QJsonArray>

GitManagerAction::GitManagerAction(QObject* parent) : BaseAgentAction(parent) {}

QString GitManagerAction::getName() const {
    return "git_manager";
}

void GitManagerAction::execute(const AgentCommand& command, const QString& workspacePath) {
    // 1. Decode the JSON array payload back into a Qt String List
    QJsonArray argsArray = QJsonDocument::fromJson(command.payload.toUtf8()).array();
    QStringList arguments;
    for (const auto& arg : argsArray) {
        arguments << arg.toString();
    }

    if (arguments.isEmpty()) {
        emit actionFinished("System Error [git_manager]: No git arguments provided.");
        return;
    }

    // 2. Execute the Git process securely
    QProcess gitProcess;
    gitProcess.setWorkingDirectory(workspacePath);
    gitProcess.start("git", arguments);

    // Wait for the command to finish (timeout after 10 seconds to prevent hanging)
    if (!gitProcess.waitForFinished(10000)) {
        gitProcess.kill();
        emit actionFinished("System Error [git_manager]: Process timed out after 10 seconds.");
        return;
    }

    // 3. Capture the terminal output
    QString stdOut = QString::fromUtf8(gitProcess.readAllStandardOutput()).trimmed();
    QString stdErr = QString::fromUtf8(gitProcess.readAllStandardError()).trimmed();

    QString finalOutput;
    if (!stdOut.isEmpty()) finalOutput += stdOut + "\n";
    if (!stdErr.isEmpty()) finalOutput += "Status/Errors:\n" + stdErr;

    if (finalOutput.trimmed().isEmpty()) {
        finalOutput = "Success (No terminal output).";
    }

    // 4. Send the output back to the router
    emit actionFinished(QString("System [git_manager]:\n%1").arg(finalOutput.trimmed()));
}