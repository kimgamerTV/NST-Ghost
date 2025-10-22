#ifndef BGADATAMANAGER_H
#define BGADATAMANAGER_H

#include <QObject>
#include <QStringList>
#include <QStandardItemModel>
#include <QMap>
#include <QPair>
#include <QJsonArray>

// Include BGACore headers
#include <core/gameanalyzer.h>
#include <core/analyzerfactory.h>

class BGADataManager : public QObject
{
    Q_OBJECT
public:
    explicit BGADataManager(QObject *parent = nullptr);

    QStringList getAvailableAnalyzers() const;
    QJsonArray loadStringsFromGameProject(const QString &engineName, const QString &projectPath);
    bool saveStringsToGameProject(const QString &engineName, const QString &projectPath, const QMap<QString, QJsonArray> &data); // New method

signals:
    void errorOccurred(const QString &message);

private:
};

#endif // BGADATAMANAGER_H