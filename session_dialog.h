/**
 * @file session_dialog.h
 * @brief Dialog window for managing local projects and chat sessions.
 *
 * This file defines the SessionDialog class and SessionData struct, 
 * which handle the creation, deletion, and selection of isolated 
 * agent workspaces and their associated SQLite chat histories.
 */

#ifndef SESSION_DIALOG_H
#define SESSION_DIALOG_H

#include <QDialog>
#include <QString>

// Forward declarations to optimize compile times and reduce header bloat
class QListWidget;
class QPushButton;

/**
 * @brief A lightweight data structure representing a saved agent session.
 */
struct SessionData {
    QString id;         ///< Unique UUID linking the session to the SQLite database
    QString name;       ///< Human-readable project/session name
    QString workspace;  ///< Absolute path to the isolated local sandbox directory
};

class SessionDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Constructs the Session Dialog and loads existing sessions from the database.
     * @param parent The parent widget, typically the MainWindow.
     */
    explicit SessionDialog(QWidget* parent = nullptr);

    /**
     * @brief Retrieves the data for the session the user just selected.
     * @return A SessionData struct containing the ID, name, and workspace path.
     */
    SessionData getSelectedSession() const;

private slots:
    // --- UI Interaction & Database Slots ---
    /**
     * @brief Queries the SQLite database and populates the list widget.
     */
    void loadSessionsFromDb();
    
    /**
     * @brief Prompts the user for a name and directory, then saves a new session.
     */
    void createNewSession();
    
    /**
     * @brief Permanently deletes the selected session and its associated chat history.
     */
    void deleteSelectedSession();
    
    /**
     * @brief Validates the selection, updates the 'last used' timestamp, and closes the dialog.
     */
    void selectAndClose();

private:
    /**
     * @brief Internal helper to ensure the SQLite 'sessions' table exists before querying.
     */
    void ensureTableExists(); 

    // --- UI Pointers ---
    QListWidget* sessionList;
    QPushButton* btnNew;
    QPushButton* btnDelete;
    QPushButton* btnLoad;
    
    // --- Internal State ---
    SessionData selectedSession;
};

#endif // SESSION_DIALOG_H