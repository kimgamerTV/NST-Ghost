#include "searchcontroller.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QFileInfo>
#include <QtConcurrent/QtConcurrent>

SearchController::SearchController(QStandardItemModel *model, QTableView *view, QObject *parent)
    : QObject(parent), m_translationModel(model), m_view(view), m_loadedGameProjectData(nullptr), m_fileListModel(nullptr)
{
}

void SearchController::setTranslationModel(QStandardItemModel *model)
{
    m_translationModel = model;
}

void SearchController::setLoadedGameProjectData(const QMap<QString, QJsonArray> *data)
{
    m_loadedGameProjectData = data;
}

void SearchController::setFileListModel(QStringListModel *model)
{
    m_fileListModel = model;
}

QList<QPair<QString, QPair<int, QString>>> SearchController::searchAllFiles(const QString &query) const
{
    if (query.isEmpty() || !m_loadedGameProjectData) {
        return {};
    }

    auto searchFile = [&](const QString &filePath) {
        QList<QPair<QString, QPair<int, QString>>> results;
        const QJsonArray &textsArray = m_loadedGameProjectData->value(filePath);
        int row = 0;
        for (const QJsonValue &value : textsArray) {
            if (value.isObject()) {
                QJsonObject textObject = value.toObject();
                QString text = textObject["text"].toString();
                QString key = textObject["key"].toString();

                if (text.contains(query, Qt::CaseInsensitive) || key.contains(query, Qt::CaseInsensitive)) {
                    results.append(qMakePair(filePath, qMakePair(row, text)));
                }
            }
            row++;
        }
        return results;
    };

    auto reduceLists = [](QList<QPair<QString, QPair<int, QString>>> &result, const QList<QPair<QString, QPair<int, QString>>> &intermediate) {
        result.append(intermediate);
    };

    return QtConcurrent::blockingMappedReduced(m_loadedGameProjectData->keys(), searchFile, reduceLists);
}

void SearchController::onSearchQueryChanged(const QString &query)
{
    if (!m_translationModel || !m_view) {
        return;
    }

    for (int i = 0; i < m_translationModel->rowCount(); ++i) {
        bool match = false;
        if (query.isEmpty()) {
            match = true;
        } else {
            for (int j = 0; j < m_translationModel->columnCount(); ++j) {
                QStandardItem *item = m_translationModel->item(i, j);
                if (item && item->text().contains(query, Qt::CaseInsensitive)) {
                    match = true;
                    break;
                }
            }
        }
        m_view->setRowHidden(i, !match);
    }
}