#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget* parent = nullptr);
    QString getApiKey() const;
    QString getWorkspaceDirectory() const;
    QString getGithubPat() const;

private slots:
    void browseWorkspace();
    void saveSettings();

private:
    QLineEdit* apiKeyInput;
    QLineEdit* workspaceInput;
    QPushButton* browseButton;
    QPushButton* saveButton;
    QLineEdit* githubPatInput;
};

#endif // SETTINGS_DIALOG_H