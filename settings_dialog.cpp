#include "settings_dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>

SettingsDialog::SettingsDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Agent Configuration");
    setMinimumWidth(400);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // API Key UI
    mainLayout->addWidget(new QLabel("Gemini API Key:"));
    apiKeyInput = new QLineEdit(this);
    apiKeyInput->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    mainLayout->addWidget(apiKeyInput);

    // GitHub PAT UI
    mainLayout->addWidget(new QLabel("GitHub Personal Access Token (Optional):"));
    githubPatInput = new QLineEdit(this);
    githubPatInput->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    githubPatInput->setPlaceholderText("ghp_xxxxxxxxxxxxxxxxxxxx");
    mainLayout->addWidget(githubPatInput);

    // Workspace UI
    mainLayout->addWidget(new QLabel("Agent Workspace (Sandbox Directory):"));
    QHBoxLayout* wsLayout = new QHBoxLayout();
    workspaceInput = new QLineEdit(this);
    workspaceInput->setReadOnly(true); // Force user to use the browse button
    browseButton = new QPushButton("Browse...", this);
    wsLayout->addWidget(workspaceInput);
    wsLayout->addWidget(browseButton);
    mainLayout->addLayout(wsLayout);

    // Save Button
    saveButton = new QPushButton("Save & Launch", this);
    mainLayout->addWidget(saveButton);

    // Load Existing Settings
    QSettings settings;
    apiKeyInput->setText(settings.value("api_key", "").toString());
    workspaceInput->setText(settings.value("workspace_dir", QDir::homePath()).toString());
    githubPatInput->setText(settings.value("github_pat", "").toString());

    // Connections
    connect(browseButton, &QPushButton::clicked, this, &SettingsDialog::browseWorkspace);
    connect(saveButton, &QPushButton::clicked, this, &SettingsDialog::saveSettings);
}

void SettingsDialog::browseWorkspace() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select Workspace Directory",
                                                    workspaceInput->text(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        workspaceInput->setText(dir);
    }
}

void SettingsDialog::saveSettings() {
    if (apiKeyInput->text().trimmed().isEmpty() || workspaceInput->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Error", "Both API Key and Workspace Directory are required.");
        return;
    }

    QSettings settings;
    settings.setValue("api_key", apiKeyInput->text().trimmed());
    settings.setValue("workspace_dir", workspaceInput->text().trimmed());
    settings.setValue("github_pat", githubPatInput->text().trimmed());

    accept();
}

QString SettingsDialog::getApiKey() const { return apiKeyInput->text().trimmed(); }
QString SettingsDialog::getWorkspaceDirectory() const { return workspaceInput->text().trimmed(); }
QString SettingsDialog::getGithubPat() const { return githubPatInput->text().trimmed(); }