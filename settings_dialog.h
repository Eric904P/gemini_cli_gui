/**
 * @file settings_dialog.h
 * @brief Dialog window for managing global application settings.
 *
 * Handles the input, validation, and persistent storage of sensitive 
 * user credentials such as the Google Gemini API Key and GitHub 
 * Personal Access Token (PAT).
 */

#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include <QDialog>
#include <QString>

// Forward declaration to reduce header dependency bloat
class QLineEdit;

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Constructs the Settings Dialog and populates it with existing saved keys.
     * @param parent The parent widget, typically the MainWindow.
     */
    explicit SettingsDialog(QWidget* parent = nullptr);

    /**
     * @brief Retrieves the user's Gemini API Key from the input field.
     * @return The sanitized API Key string.
     */
    QString getApiKey() const;

    /**
     * @brief Retrieves the user's GitHub Personal Access Token (PAT).
     * @return The sanitized PAT string.
     */
    QString getGithubPat() const;

private slots:
    /**
     * @brief Persists the active input values to the local OS settings registry.
     */
    void saveSettings();

private:
    QLineEdit* apiKeyInput;     ///< Input field for the Gemini API Key
    QLineEdit* githubPatInput;  ///< Input field for the GitHub PAT
};

#endif // SETTINGS_DIALOG_H