#include "translationserver.h"
#include <QDataStream>

TranslationServer::TranslationServer(QObject *parent)
    : QObject(parent), m_server(new QTcpServer(this))
{
    connect(m_server, &QTcpServer::newConnection, this, &TranslationServer::onNewConnection);
}

TranslationServer::~TranslationServer()
{
    stopServer();
}

bool TranslationServer::startServer(quint16 port)
{
    if (m_server->isListening()) {
        return true;
    }

    if (m_server->listen(QHostAddress::Any, port)) {
        emit logMessage(QString("Server started on port %1").arg(port));
        return true;
    } else {
        emit logMessage(QString("Server failed to start: %1").arg(m_server->errorString()));
        return false;
    }
}

void TranslationServer::stopServer()
{
    if (m_server->isListening()) {
        for (QTcpSocket *socket : m_clients) {
            socket->disconnectFromHost();
        }
        m_server->close();
        emit logMessage("Server stopped");
    }
}

void TranslationServer::onNewConnection()
{
    QTcpSocket *clientSocket = m_server->nextPendingConnection();
    m_clients.append(clientSocket);

    connect(clientSocket, &QTcpSocket::readyRead, this, &TranslationServer::onReadyRead);
    connect(clientSocket, &QTcpSocket::disconnected, this, &TranslationServer::onClientDisconnected);

    emit logMessage("New client connected");
}

void TranslationServer::onReadyRead()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) return;

    // Simple protocol: waiting for raw text or JSON
    // For MTool compatibility, we might need to inspect the protocol more closely later.
    // For now, let's assume raw text lines or JSON objects sent as bytes.
    
    QByteArray data = clientSocket->readAll();
    QString text = QString::fromUtf8(data).trimmed();

    if (!text.isEmpty()) {
        emit newTranslationRequest(text, clientSocket);
    }
}

void TranslationServer::onClientDisconnected()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (clientSocket) {
        m_clients.removeAll(clientSocket);
        clientSocket->deleteLater();
        emit logMessage("Client disconnected");
    }
}
