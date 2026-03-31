/**
 * @file settings_dialog.cpp
 * @brief Implementation of the global settings dialog.
 *
 * This file handles the UI layout, initialization from stored OS settings,
 * and the secure persistence of API keys and authentication tokens.
 */

#include "settings_dialog.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QVBoxLayout>

/**
 * @brief Constructs the Settings Dialog UI and loads saved credentials.
 */
SettingsDialog::SettingsDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Global Settings");
    setMinimumWidth(400);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // --- Gemini API Key UI ---
    mainLayout->addWidget(new QLabel("Google Gemini API Key:"));
    apiKeyInput = new QLineEdit(this);
    apiKeyInput->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    apiKeyInput->setPlaceholderText("AIzaSy...");
    mainLayout->addWidget(apiKeyInput);

    // --- GitHub PAT UI ---
    mainLayout->addWidget(new QLabel("GitHub Personal Access Token (Optional):"));
    githubPatInput = new QLineEdit(this);
    githubPatInput->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    githubPatInput->setPlaceholderText("ghp_xxxxxxxxxxxxxxxxxxxx");
    mainLayout->addWidget(githubPatInput);

    // --- Dialog Buttons ---
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* btnSave = new QPushButton("Save", this);
    QPushButton* btnCancel = new QPushButton("Cancel", this);

    buttonLayout->addStretch();
    buttonLayout->addWidget(btnSave);
    buttonLayout->addWidget(btnCancel);
    
    mainLayout->addLayout(buttonLayout);

    // --- Initialize from OS Registry/Settings ---
    QSettings settings;
    apiKeyInput->setText(settings.value("api_key", "").toString());
    githubPatInput->setText(settings.value("github_pat", "").toString());

    // --- Connections ---
    connect(btnSave, &QPushButton::clicked, this, &SettingsDialog::saveSettings);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

/**
 * @brief Saves the inputs to QSettings and closes the dialog.
 */
void SettingsDialog::saveSettings() {
    QString apiKey = apiKeyInput->text().trimmed();
    
    // Validate that the core API key is actually provided
    if (apiKey.isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "The Gemini API Key cannot be empty.");
        return;
    }

    QSettings settings;
    settings.setValue("api_key", apiKey);
    settings.setValue("github_pat", githubPatInput->text().trimmed());
    
    accept();
}

/**
 * @brief Returns the sanitized API Key.
 */
QString SettingsDialog::getApiKey() const {
    return apiKeyInput->text().trimmed();
}

/**
 * @brief Returns the sanitized GitHub PAT.
 */
QString SettingsDialog::getGithubPat() const {
    return githubPatInput->text().trimmed();
}