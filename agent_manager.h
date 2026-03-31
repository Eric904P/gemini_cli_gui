/**
 * @file agent_manager.h
 * @brief Manages the execution of local machine actions requested by the agent.
 *
 * Defines the AgentCommand structure and the AgentActionManager class,
 * which enforces security whitelists and executes approved local tasks
 * such as writing files to the disk.
 */

#ifndef AGENT_MANAGER_H
#define AGENT_MANAGER_H

#include <QObject>
#include <QString>
#include <QSet>

/**
 * @brief Data structure representing a tool execution request from the LLM.
 */
struct AgentCommand {
    QString action;   ///< The name of the tool/function to execute (e.g., "write_file")
    QString target;   ///< The file path, URL, or terminal command string
    QString payload;  ///< Optional data payload (e.g., file contents to write, JSON args)
};

class AgentActionManager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructs the agent action manager.
     * @param parent The parent QObject, typically the MainWindow.
     */
    explicit AgentActionManager(QObject* parent = nullptr);

    /**
     * @brief Adds an action to the safe-list of capabilities.
     * @param action The function name to whitelist (e.g., "write_file").
     */
    void addWhitelistedAction(const QString& action);

    /**
     * @brief Checks if a requested action is in the approved whitelist.
     * @param action The function name to verify.
     * @return True if the action is safely whitelisted, false otherwise.
     */
    bool isActionWhitelisted(const QString& action) const;

    /**
     * @brief Executes the validated and approved command on the local machine.
     * @param command The fully populated agent command struct.
     * @return True if the file or action was successfully written/executed.
     */
    bool executeApprovedAction(const AgentCommand& command);

private:
    QSet<QString> whitelistedActions; ///< Collection of pre-approved safe actions
};

#endif // AGENT_MANAGER_H