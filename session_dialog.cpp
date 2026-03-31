/**
 * @file session_dialog.cpp
 * @brief Implementation of the local project and session manager.
 *
 * This file handles the UI for creating, loading, and deleting isolated 
 * workspaces. It manages the SQLite 'sessions' table and ensures 
 * seamless project hot-swapping.
 */

#include "session_dialog.h"

#include <QDateTime>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlError>
#include <QSqlQuery>
#include <QUuid>
#include <QVBoxLayout>

/**
 * @brief Constructs the Session Dialog, initializes the UI, and loads history.
 */
SessionDialog::SessionDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Project & Session Manager");
    setMinimumSize(400, 300);

    ensureTableExists();

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // --- Session List View ---
    sessionList = new QListWidget(this);
    mainLayout->addWidget(sessionList);

    // --- Control Buttons ---
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnNew = new QPushButton("New Session", this);
    btnDelete = new QPushButton("Delete", this);
    btnLoad = new QPushButton("Load Selected", this);

    // Allow the user to hit 'Enter' to quickly load the highlighted session
    btnLoad->setDefault(true); 

    btnLayout->addWidget(btnNew);
    btnLayout->addWidget(btnDelete);
    btnLayout->addStretch();
    btnLayout->addWidget(btnLoad);

    mainLayout->addLayout(btnLayout);

    // --- Connections ---
    connect(btnNew, &QPushButton::clicked, this, &SessionDialog::createNewSession);
    connect(btnDelete, &QPushButton::clicked, this, &SessionDialog::deleteSelectedSession);
    connect(btnLoad, &QPushButton::clicked, this, &SessionDialog::selectAndClose);
    connect(sessionList, &QListWidget::itemDoubleClicked, this, &SessionDialog::selectAndClose);

    loadSessionsFromDb();
}

/**
 * @brief Ensures the necessary SQLite table exists before querying.
 */
void SessionDialog::ensureTableExists() {
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS sessions ("
               "id TEXT PRIMARY KEY, "
               "name TEXT, "
               "workspace TEXT, "
               "created_at INTEGER)");
}

/**
 * @brief Retrieves all sessions from the database, sorted by most recently used.
 */
void SessionDialog::loadSessionsFromDb() {
    sessionList->clear();
    QSqlQuery query("SELECT id, name, workspace FROM sessions ORDER BY created_at DESC");
    
    while (query.next()) {
        QString id = query.value(0).toString();
        QString name = query.value(1).toString();
        QString workspace = query.value(2).toString();
        
        QListWidgetItem* item = new QListWidgetItem(QString("%1\n(%2)").arg(name, workspace));
        
        // Store the hidden data payload inside the UI item
        item->setData(Qt::UserRole, id);
        item->setData(Qt::UserRole + 1, name);
        item->setData(Qt::UserRole + 2, workspace);
        
        sessionList->addItem(item);
    }

    // Auto-select the most recently used session for lightning-fast startups
    if (sessionList->count() > 0) {
        sessionList->setCurrentRow(0); 
    }
}

/**
 * @brief Prompts the user to create a new session and local workspace mapping.
 */
void SessionDialog::createNewSession() {
    // 1. Ask for a project name
    bool ok;
    QString name = QInputDialog::getText(this, "New Session", "Enter project/session name:", QLineEdit::Normal, "", &ok);
    if (!ok || name.trimmed().isEmpty()) return;

    // 2. Ask for the workspace directory to sandbox the agent
    QString workspace = QFileDialog::getExistingDirectory(this, "Select Workspace Directory for this Session");
    if (workspace.isEmpty()) return;

    // 3. Generate a unique ID and save to database
    QString newId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    
    QSqlQuery query;
    query.prepare("INSERT INTO sessions (id, name, workspace, created_at) VALUES (:id, :name, :ws, :time)");
    query.bindValue(":id", newId);
    query.bindValue(":name", name);
    query.bindValue(":ws", workspace);
    query.bindValue(":time", QDateTime::currentSecsSinceEpoch());
    
    if (query.exec()) {
        loadSessionsFromDb(); 
    } else {
        QMessageBox::critical(this, "Database Error", "Failed to create session: " + query.lastError().text());
    }
}

/**
 * @brief Deletes a session and wipes its conversational memory from the database.
 * @note This does NOT delete the actual local files in the workspace.
 */
void SessionDialog::deleteSelectedSession() {
    QListWidgetItem* item = sessionList->currentItem();
    if (!item) return;

    QString sessionId = item->data(Qt::UserRole).toString();

    int ret = QMessageBox::warning(this, "Delete Session", 
                                   "Are you sure you want to delete this session and its chat history?\n(Local files will NOT be deleted).",
                                   QMessageBox::Yes | QMessageBox::No);
                                   
    if (ret == QMessageBox::Yes) {
        QSqlQuery query;
        
        // Delete the master session record
        query.prepare("DELETE FROM sessions WHERE id = :id");
        query.bindValue(":id", sessionId);
        query.exec();
        
        // Wipe the associated chat history so it doesn't leave orphaned data
        query.prepare("DELETE FROM interactions WHERE session_id = :id");
        query.bindValue(":id", sessionId);
        query.exec();

        loadSessionsFromDb();
    }
}

/**
 * @brief Validates selection, updates the 'last used' timestamp, and accepts the dialog.
 */
void SessionDialog::selectAndClose() {
    QListWidgetItem* item = sessionList->currentItem();
    if (!item) {
        QMessageBox::information(this, "Notice", "Please select a session or create a new one.");
        return;
    }

    selectedSession.id = item->data(Qt::UserRole).toString();
    selectedSession.name = item->data(Qt::UserRole + 1).toString();
    selectedSession.workspace = item->data(Qt::UserRole + 2).toString();
    
    // Update the timestamp so this project bubbles to the top of the list next time!
    QSqlQuery query;
    query.prepare("UPDATE sessions SET created_at = :time WHERE id = :id");
    query.bindValue(":time", QDateTime::currentSecsSinceEpoch());
    query.bindValue(":id", selectedSession.id);
    query.exec();
    
    accept();
}

/**
 * @brief Returns the payload for the application to instantiate the workspace.
 */
SessionData SessionDialog::getSelectedSession() const {
    return selectedSession;
}