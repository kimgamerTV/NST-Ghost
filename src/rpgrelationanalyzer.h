#ifndef RPGRELATIONANALYZER_H
#define RPGRELATIONANALYZER_H

#include <QString>
#include <QList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>

struct RpgDependency {
    QString sourceId;
    QString sourceType;
    QString sourceLabel;
    
    QString targetId;
    QString targetType;
    QString targetLabel;
    
    QString relationType; // "Calls", "Toggles", "Modifies"
};

class RpgRelationAnalyzer
{
public:
    RpgRelationAnalyzer(const QString &projectPath);
    
    QList<RpgDependency> analyze();

private:
    QString m_projectPath;
    
    void analyzeCommonEvents(QList<RpgDependency> &deps);
    // Future: analyzeMapEvents, etc.
    
    QString getCommonEventName(int id, const QJsonArray &data);
    QString getSwitchName(int id); // Requires parsing System.json
    QString getVariableName(int id); // Requires parsing System.json
    
    // Cache for names
    QMap<int, QString> m_commonEventNames;
    QMap<int, QString> m_switchNames;
    QMap<int, QString> m_variableNames;
    
    void loadSystemData();
};

#endif // RPGRELATIONANALYZER_H
