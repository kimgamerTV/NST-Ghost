#ifndef SEARCHCONTROLLER_H
#define SEARCHCONTROLLER_H

#include <QObject>
#include <QStandardItemModel>
#include <QTableView>
#include <QJsonArray>
#include <QMap>
#include <QStringListModel>
#include <QPair>

class SearchController : public QObject
{
    Q_OBJECT
public:
    explicit SearchController(QStandardItemModel *model, QTableView *view, QObject *parent = nullptr);

    void setTranslationModel(QStandardItemModel *model);
    void setLoadedGameProjectData(const QMap<QString, QJsonArray> *data);
    void setFileListModel(QStringListModel *model);

    QList<QPair<QString, QPair<int, QString>>> searchAllFiles(const QString &query) const;
    QList<QPair<QString, QPair<int, QString>>> searchCurrentTable(const QString &query) const;

public slots:
    void onSearchQueryChanged(const QString &query);

private:
    QStandardItemModel *m_translationModel;
    QTableView *m_view;
    const QMap<QString, QJsonArray> *m_loadedGameProjectData;
    QStringListModel *m_fileListModel;
};

#endif // SEARCHCONTROLLER_H