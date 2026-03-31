#ifndef SESSION_DIALOG_H
#define SESSION_DIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QString>

// A helper struct to pass session data around
struct SessionData {
    QString id;
    QString name;
    QString workspace;
};

class SessionDialog : public QDialog {
    Q_OBJECT

public:
    explicit SessionDialog(QWidget* parent = nullptr);
    SessionData getSelectedSession() const;

private slots:
    void loadSessionsFromDb();
    void createNewSession();
    void deleteSelectedSession();
    void selectAndClose();

private:
    QListWidget* sessionList;
    QPushButton* btnNew;
    QPushButton* btnDelete;
    QPushButton* btnLoad;
    
    SessionData selectedSession;
    
    // Internal helper to ensure the DB table exists
    void ensureTableExists(); 
};

#endif // SESSION_DIALOG_H