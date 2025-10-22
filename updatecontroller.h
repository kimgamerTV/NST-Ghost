#ifndef UPDATECONTROLLER_H
#define UPDATECONTROLLER_H

#include <QObject>
#include <QNetworkAccessManager>

class UpdateController : public QObject
{
    Q_OBJECT
public:
    explicit UpdateController(QObject *parent = nullptr);

    void checkForUpdates();

signals:

private slots:
    void onManifestDownloaded(QNetworkReply *reply);

private:
    void parseManifest(const QByteArray &manifestData);
    void downloadFile(const QUrl &url, const QString &filePath);

    QNetworkAccessManager *m_networkManager;
    QUrl m_manifestUrl;
};

#endif // UPDATECONTROLLER_H
