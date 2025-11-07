#ifndef PROJECTDATAMANAGER_H
#define PROJECTDATAMANAGER_H

#include <QObject>
#include <QStandardItemModel>
#include <QStringListModel>
#include <QMap>
#include <QJsonArray>
#include <QJsonObject>
#include <QFutureWatcher>

class ProjectDataManager : public QObject
{
    Q_OBJECT
public:
    explicit ProjectDataManager(QStringListModel *fileListModel, QStandardItemModel *translationModel, QObject *parent = nullptr);

    QMap<QString, QJsonArray> &getLoadedGameProjectData();
    QString &getCurrentLoadedFilePath();

public slots:
    void onLoadingFinished(const QJsonArray &extractedTextsArray);
    void onFileSelected(const QModelIndex &index);

private:
    QStringListModel *m_fileListModel;
    QStandardItemModel *m_translationModel;
    QMap<QString, QJsonArray> m_loadedGameProjectData;
    QString m_currentLoadedFilePath;
};

#endif // PROJECTDATAMANAGER_H
