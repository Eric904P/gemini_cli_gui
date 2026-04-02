/**
 * @file git_manager_action.h
 * @brief Autonomous tool for the LLM to interact with Git version control.
 */

#ifndef GIT_MANAGER_ACTION_H
#define GIT_MANAGER_ACTION_H

#include "base_agent_action.h"
#include <QObject>

class GitManagerAction : public BaseAgentAction {
    Q_OBJECT
public:
    explicit GitManagerAction(QObject* parent = nullptr);

    QString getName() const override;
    void execute(const AgentCommand& command, const QString& workspacePath) override;
};

#endif // GIT_MANAGER_ACTION_H