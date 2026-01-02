#ifndef TRANSLATIONSERVER_H
#define TRANSLATIONSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class TranslationServer : public QObject
{
    Q_OBJECT
public:
    explicit TranslationServer(QObject *parent = nullptr);
    ~TranslationServer();

    bool startServer(quint16 port = 14478);
    void stopServer();

signals:
    void newTranslationRequest(const QString &sourceText, QTcpSocket *socket);
    void logMessage(const QString &message);

private slots:
    void onNewConnection();
    void onReadyRead();
    void onClientDisconnected();

private:
    QTcpServer *m_server;
    QList<QTcpSocket*> m_clients;
};

#endif // TRANSLATIONSERVER_H
