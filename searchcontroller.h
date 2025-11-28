#ifndef SEARCHCONTROLLER_H
#define SEARCHCONTROLLER_H

#include <QObject>
#include <QStandardItemModel>
#include <QTableView>
#include <QJsonArray>
#include <QMap>
#include <QStringListModel>
#include <QPair>
#include <QtConcurrent/QtConcurrent>

class SearchController : public QObject
{
    Q_OBJECT
public:
    explicit SearchController(QStandardItemModel *model, QTableView *view, QObject *parent = nullptr);

    void setTranslationModel(QStandardItemModel *model);
    void setLoadedGameProjectData(const QMap<QString, QJsonArray> *data);
    void setFileListModel(QStandardItemModel *model);

    QList<QPair<QString, QPair<int, QString>>> searchAllFiles(const QString &query) const;
    void setHideCompleted(bool hide); // New method
    QString currentQuery() const { return m_currentQuery; }

public slots:
    void onSearchQueryChanged(const QString &query);

private:
    QStandardItemModel *m_translationModel;
    QTableView *m_view;
    const QMap<QString, QJsonArray> *m_loadedGameProjectData;
    QStandardItemModel *m_fileListModel;
    bool m_hideCompleted = false; // New member
    QString m_currentQuery; // Store current query to re-apply filter
};

#endif // SEARCHCONTROLLER_H