/**
 * @file gemini_api_client.h
 * @brief Network client for communicating with the Google Gemini API.
 *
 * This class manages the stateful interaction with the Gemini v1beta API,
 * including JSON payload construction, multi-modal file attachments, 
 * tool definitions, and parsing Google's asynchronous responses.
 */

#ifndef GEMINI_API_CLIENT_H
#define GEMINI_API_CLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QStringList>

class GeminiApiClient : public QObject {
    Q_OBJECT

public:
    explicit GeminiApiClient(QObject* parent = nullptr);
    ~GeminiApiClient();

    // --- Configuration ---
    void setApiKey(const QString& key);
    
    // --- Core Execution ---
    /**
     * @brief Sends a prompt and optional file attachments to the Gemini API.
     */
    void sendPrompt(const QString& userInput, const QStringList& attachments = QStringList());

    // --- Session Management ---
    /**
     * @brief Clears the current interaction ID to start a fresh thread.
     */
    void resetSession();

    /**
     * @brief Restores the interaction ID from SQLite to resume a previous chat.
     */
    void restoreSession(const QString& interactionId);

signals:
    // --- API Response Signals ---
    void responseReceived(const QString& responseText, const QString& interactionId);
    void networkError(const QString& errorDetails);
    void functionCallRequested(const QString& functionName, const QJsonObject& arguments);
    void usageMetricsReceived(int inputTokens, int outputTokens, int totalTokens);

private slots:
    // --- Internal Network Handlers ---
    void onNetworkReply(QNetworkReply* reply);

private:
    QNetworkAccessManager* networkManager;
    QString apiKey;
    
    /// Tracks the stateful API thread to maintain chat history on Google's servers
    QString currentApiInteractionId; 
    
    // --- Payload Construction Helpers ---
    QNetworkRequest createRequest() const;
    QByteArray buildJsonPayload(const QString& newPrompt, const QStringList& attachments);
    QJsonArray defineAvailableTools() const;
};

#endif // GEMINI_API_CLIENT_H