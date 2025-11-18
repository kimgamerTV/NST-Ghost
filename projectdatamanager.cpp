#include "projectdatamanager.h"

#include <QFileInfo>
#include <QDebug>
#include <QSet>
#include <QtConcurrent>

ProjectDataManager::ProjectDataManager(QStandardItemModel *fileListModel, QStandardItemModel *translationModel, QObject *parent)
    : QObject(parent)
    , m_fileListModel(fileListModel)
    , m_translationModel(translationModel)
{
    connect(&m_processingFutureWatcher, &QFutureWatcher<QPair<QMap<QString, QJsonArray>, QStringList>>::finished, this, &ProjectDataManager::onProcessingFinished);
}

QMap<QString, QJsonArray> &ProjectDataManager::getLoadedGameProjectData()
{
    return m_loadedGameProjectData;
}

QString &ProjectDataManager::getCurrentLoadedFilePath()
{
    return m_currentLoadedFilePath;
}

void ProjectDataManager::onLoadingFinished(const QJsonArray &extractedTextsArray)
{
    qDebug() << "ProjectDataManager: onLoadingFinished called with " << extractedTextsArray.size() << " entries. Starting background processing.";

    QFuture<QPair<QMap<QString, QJsonArray>, QStringList>> future = QtConcurrent::run([extractedTextsArray]() {
        qDebug() << "ProjectDataManager (background): Starting processing of" << extractedTextsArray.size() << "entries.";
        QMap<QString, QJsonArray> fileMap;
        QSet<QString> filePaths;

        // Log first 5 entries for inspection
        for (int i = 0; i < 5 && i < extractedTextsArray.size(); ++i) {
            qDebug() << "ProjectDataManager (background): Sample entry" << i << ":" << extractedTextsArray.at(i).toObject();
        }

        for (const QJsonValue &value : extractedTextsArray) {
            QJsonObject obj = value.toObject();
            QString filePath = obj["path"].toString();
            if (filePath.isEmpty()) {
                // Let's log if we find an empty file path
                if (filePaths.isEmpty()) { // Log only a few times to avoid spam
                     qDebug() << "ProjectDataManager (background): Found entry with empty 'path' key. Object:" << obj;
                }
                continue;
            }

            fileMap[filePath].append(obj);
            filePaths.insert(filePath);
        }

        // Sort by filename
        QStringList sortedPaths = filePaths.values();
        std::sort(sortedPaths.begin(), sortedPaths.end(), [](const QString &a, const QString &b) {
            return QFileInfo(a).fileName() < QFileInfo(b).fileName();
        });

        qDebug() << "ProjectDataManager (background): Finished processing. Found" << sortedPaths.size() << "unique files.";
        return qMakePair(fileMap, sortedPaths);
    });

    m_processingFutureWatcher.setFuture(future);
}

void ProjectDataManager::onProcessingFinished()
{
    qDebug() << "ProjectDataManager: Background processing finished.";
    QPair<QMap<QString, QJsonArray>, QStringList> result = m_processingFutureWatcher.result();
    m_loadedGameProjectData = result.first;
    
    m_fileListModel->clear();
    for (const QString &path : result.second) {
        QStandardItem *item = new QStandardItem(QFileInfo(path).fileName());
        item->setData(path, Qt::UserRole);
        m_fileListModel->appendRow(item);
    }

    qDebug() << "ProjectDataManager: Models updated. Emitting processingFinished signal.";
    emit processingFinished();
}


void ProjectDataManager::onFileSelected(const QModelIndex &index)
{
    QStandardItem *item = m_fileListModel->itemFromIndex(index);
    if (!item) return;
    
    QString fullFilePath = item->data(Qt::UserRole).toString();

    m_translationModel->clear();
    m_translationModel->setHorizontalHeaderLabels(QStringList() << "Source Text" << "Translation");

    m_currentLoadedFilePath = fullFilePath;

    if (m_loadedGameProjectData.contains(fullFilePath)) {
        const QJsonArray &textsArray = m_loadedGameProjectData.value(fullFilePath);
        if (textsArray.isEmpty()) return;

        // Temporarily disable updates for performance
        // This will be handled by MainWindow's QTableView directly
        // For now, we'll just populate the model

        for (const QJsonValue &value : textsArray) {
            QJsonObject obj = value.toObject();
            QString source = obj["source"].toString();
            QString translation = obj["text"].toString();
            QString key = obj["key"].toString();

            QStandardItem *sourceItem = new QStandardItem(source);
            QStandardItem *transItem = new QStandardItem(translation);

            // เก็บ key เพื่อใช้ตอน save
            sourceItem->setData(key, Qt::UserRole + 1);

            m_translationModel->appendRow(QList<QStandardItem*>() << sourceItem << transItem);
        }
    }
}
