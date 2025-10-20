#include "searchcontroller.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QFileInfo>

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
    QList<QPair<QString, QPair<int, QString>>> results;

    if (query.isEmpty() || !m_loadedGameProjectData || !m_fileListModel) {
        return results;
    }

    for (const QString &filePath : m_loadedGameProjectData->keys()) {
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
    }
    return results;
}

QList<QPair<QString, QPair<int, QString>>> SearchController::searchCurrentTable(const QString &query) const
{
    QList<QPair<QString, QPair<int, QString>>> results;

    if (query.isEmpty() || !m_translationModel) {
        return results;
    }

    // Get the currently selected file name from the file list model
    // This is a bit of a hack, as SearchController shouldn't directly know about UI selection
    // But for now, to get the file context for search results, we'll do this.
    QString currentFileName = "";
    if (m_fileListModel && m_fileListModel->rowCount() > 0) {
        currentFileName = m_fileListModel->data(m_fileListModel->index(0,0), Qt::DisplayRole).toString();
    }

    for (int i = 0; i < m_translationModel->rowCount(); ++i) {
        for (int j = 0; j < m_translationModel->columnCount(); ++j) {
            QStandardItem *item = m_translationModel->item(i, j);
            if (item && item->text().contains(query, Qt::CaseInsensitive)) {
                results.append(qMakePair(currentFileName, qMakePair(i, item->text())));
                break;
            }
        }
    }
    return results;
}

void SearchController::onSearchQueryChanged(const QString &query)
{
    // This slot is primarily for filtering the currently displayed table
    // The search dialog will handle searching across all files.

    if (!m_translationModel || !m_view) {
        return;
    }

    for (int i = 0; i < m_translationModel->rowCount(); ++i) {
        bool match = false;
        for (int j = 0; j < m_translationModel->columnCount(); ++j) {
            QStandardItem *item = m_translationModel->item(i, j);
            if (item && item->text().contains(query, Qt::CaseInsensitive)) {
                match = true;
                break;
            }
        }
        m_view->setRowHidden(i, !match);
    }
}