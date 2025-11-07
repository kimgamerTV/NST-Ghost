#include "projectdatamanager.h"

#include <QFileInfo>
#include <QDebug>
#include <QSet>

ProjectDataManager::ProjectDataManager(QStringListModel *fileListModel, QStandardItemModel *translationModel, QObject *parent)
    : QObject(parent)
    , m_fileListModel(fileListModel)
    , m_translationModel(translationModel)
{
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
    if (extractedTextsArray.isEmpty()) {
        qDebug() << "ProjectDataManager: No texts extracted or error occurred.";
        return;
    }

    QMap<QString, QJsonArray> fileMap;
    QSet<QString> fileNames;

    for (const QJsonValue &value : extractedTextsArray) {
        QJsonObject obj = value.toObject();
        QString filePath = obj["file"].toString();
        if (filePath.isEmpty()) continue;

        fileMap[filePath].append(obj);

        QString fileName = QFileInfo(filePath).fileName();
        fileNames.insert(fileName);
    }

    m_loadedGameProjectData = fileMap;
    m_fileListModel->setStringList(fileNames.values());

    if (!fileNames.isEmpty()) {
        // Auto-select first file
        // This part will be handled by MainWindow connecting to a signal from here
        // For now, we'll just log that data is ready.
        qDebug() << "ProjectDataManager: Data loaded and ready for display.";
    }
}

void ProjectDataManager::onFileSelected(const QModelIndex &index)
{
    QString fileName = m_fileListModel->data(index, Qt::DisplayRole).toString();

    m_translationModel->clear();
    m_translationModel->setHorizontalHeaderLabels(QStringList() << "Source Text" << "Translation");

    // Find the full file path from the display name
    QString fullFilePath = fileName;
    for (const QString &key : m_loadedGameProjectData.keys()) {
        if (QFileInfo(key).fileName() == fileName) {
            fullFilePath = key;
            break;
        }
    }

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
