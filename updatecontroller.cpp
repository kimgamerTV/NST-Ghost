#include "updatecontroller.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QApplication>
#include <QMessageBox>
#include <QCryptographicHash>
#include <QFile>
#include <QDir>

UpdateController::UpdateController(QObject *parent) : QObject(parent)
{
    m_networkManager = new QNetworkAccessManager(this);
    // TODO: Replace with the actual URL to your manifest file
    m_manifestUrl = QUrl("https://gist.githubusercontent.com/your-username/your-gist-id/raw/manifest.json");
}

void UpdateController::checkForUpdates()
{
    QNetworkRequest request(m_manifestUrl);
    QNetworkReply *reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, &UpdateController::onManifestDownloaded);
}

void UpdateController::onManifestDownloaded(QNetworkReply *reply)
{
    if (reply->error()) {
        qDebug() << "Error downloading manifest:" << reply->errorString();
        return;
    }

    parseManifest(reply->readAll());
    reply->deleteLater();
}

void UpdateController::parseManifest(const QByteArray &manifestData)
{
    QJsonDocument doc = QJsonDocument::fromJson(manifestData);
    if (doc.isNull()) {
        qDebug() << "Error: Invalid manifest file.";
        return;
    }

    QJsonObject manifest = doc.object();
    QString latestVersion = manifest["version"].toString();

    if (latestVersion > APP_VERSION) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::information(nullptr, "Update Available", 
                                       QString("A new version (%1) is available. Do you want to update?").arg(latestVersion), 
                                       QMessageBox::Yes|QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            QJsonObject files = manifest["files"].toObject();
            for (const QString &fileName : files.keys()) {
                QJsonObject fileInfo = files[fileName].toObject();
                QString fileHash = fileInfo["hash"].toString();
                QString fileUrl = fileInfo["url"].toString();

                // Check if the file exists and if the hash is different
                QString filePath = QApplication::applicationDirPath() + QDir::separator() + fileName;
                if (QFile::exists(filePath)) {
                    QFile file(filePath);
                    if (file.open(QIODevice::ReadOnly)) {
                        QCryptographicHash hash(QCryptographicHash::Sha256);
                        hash.addData(&file);
                        if (hash.result().toHex() != fileHash) {
                            downloadFile(QUrl(fileUrl), filePath);
                        }
                    }
                } else {
                    downloadFile(QUrl(fileUrl), filePath);
                }
            }
        }
    }
}

void UpdateController::downloadFile(const QUrl &url, const QString &filePath)
{
    QNetworkRequest request(url);
    QNetworkReply *reply = m_networkManager->get(request);

    connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error()) {
            qDebug() << "Error downloading file:" << reply->errorString();
            return;
        }

        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(reply->readAll());
            file.close();
        }

        reply->deleteLater();
    });
}