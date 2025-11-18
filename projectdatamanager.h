#ifndef PROJECTDATAMANAGER_H
#define PROJECTDATAMANAGER_H

#include <QObject>
#include <QStandardItemModel>
#include <QStringListModel>
#include <QMap>
#include <QJsonArray>
#include <QJsonObject>
#include <QFutureWatcher>
#include <QPair>
#include <QSet>
#include <QStringList>

class ProjectDataManager : public QObject
{
    Q_OBJECT
public:
    explicit ProjectDataManager(QStandardItemModel *fileListModel, QStandardItemModel *translationModel, QObject *parent = nullptr);

    QMap<QString, QJsonArray> &getLoadedGameProjectData();
    QString &getCurrentLoadedFilePath();

public slots:
    void onLoadingFinished(const QJsonArray &extractedTextsArray);
    void onFileSelected(const QModelIndex &index);

private slots:
    void onProcessingFinished();

signals:
    void processingFinished();

private:
    QStandardItemModel *m_fileListModel;
    QStandardItemModel *m_translationModel;
    QMap<QString, QJsonArray> m_loadedGameProjectData;
    QString m_currentLoadedFilePath;
    QFutureWatcher<QPair<QMap<QString, QJsonArray>, QStringList>> m_processingFutureWatcher;
};

#endif // PROJECTDATAMANAGER_H
