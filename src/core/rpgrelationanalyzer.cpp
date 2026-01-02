#include "rpgrelationanalyzer.h"
#include <QDir>

RpgRelationAnalyzer::RpgRelationAnalyzer(const QString &projectPath)
    : m_projectPath(projectPath)
{
    loadSystemData();
}

void RpgRelationAnalyzer::loadSystemData()
{
    QFile file(m_projectPath + "/data/System.json");
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonObject root = doc.object();
        
        QJsonArray switches = root["switches"].toArray();
        for (int i = 1; i < switches.size(); ++i) { // 1-based
            QString name = switches[i].toString();
            if (!name.isEmpty()) m_switchNames[i] = name;
        }
        
        QJsonArray variables = root["variables"].toArray();
        for (int i = 1; i < variables.size(); ++i) {
             QString name = variables[i].toString();
             if (!name.isEmpty()) m_variableNames[i] = name;
        }
    }
}

QList<RpgDependency> RpgRelationAnalyzer::analyze()
{
    QList<RpgDependency> deps;
    analyzeCommonEvents(deps);
    return deps;
}

void RpgRelationAnalyzer::analyzeCommonEvents(QList<RpgDependency> &deps)
{
    QFile file(m_projectPath + "/data/CommonEvents.json");
    if (!file.open(QIODevice::ReadOnly)) return;
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonArray commonEvents = doc.array();
    
    // Fill cache first
    for (int i = 1; i < commonEvents.size(); ++i) {
        QJsonObject ce = commonEvents[i].toObject();
        if (!ce.isEmpty()) {
            m_commonEventNames[ce["id"].toInt()] = ce["name"].toString();
        }
    }
    
    // Analyze commands
    for (int i = 1; i < commonEvents.size(); ++i) {
        QJsonObject ce = commonEvents[i].toObject();
        if (ce.isEmpty()) continue;
        
        int sourceIdInt = ce["id"].toInt();
        QString sourceName = ce["name"].toString();
        if (sourceName.isEmpty()) sourceName = QString("CommonEvent %1").arg(sourceIdInt);
        
        QString sourceId = QString("ce_%1").arg(sourceIdInt);
        
        // Add the CE itself as a node if we want, but usually connections define it.
        // We will add it implicitly via edges.
        
        QJsonArray list = ce["list"].toArray();
        for (const QJsonValue &val : list) {
            QJsonObject cmd = val.toObject();
            int code = cmd["code"].toInt();
            QJsonArray params = cmd["parameters"].toArray();
            
            if (code == 117) { // Common Event Call
                int targetIdInt = params[0].toInt();
                QString targetId = QString("ce_%1").arg(targetIdInt);
                QString targetName = m_commonEventNames.value(targetIdInt, QString("CommonEvent %1").arg(targetIdInt));
                
                deps.append({
                    sourceId, "CommonEvent", sourceName,
                    targetId, "CommonEvent", targetName,
                    "Calls"
                });
            } else if (code == 121) { // Control Switches
                int startId = params[0].toInt();
                int endId = params[1].toInt();
                int operation = params[2].toInt(); // 0 = ON, 1 = OFF
                
                for (int pid = startId; pid <= endId; ++pid) {
                    QString targetId = QString("sw_%1").arg(pid);
                    QString targetName = m_switchNames.value(pid, QString("Switch %1").arg(pid));
                    QString rel = (operation == 0) ? "Turns ON" : "Turns OFF";
                    
                    deps.append({
                        sourceId, "CommonEvent", sourceName,
                        targetId, "Switch", targetName,
                        rel
                    });
                }
            } else if (code == 122) { // Control Variables
                int startId = params[0].toInt();
                int endId = params[1].toInt();
                // Operation is complex (set, add, sub, mul, div, mod)
                // Just generic "Modifies" for now
                
                for (int pid = startId; pid <= endId; ++pid) {
                    QString targetId = QString("var_%1").arg(pid);
                    QString targetName = m_variableNames.value(pid, QString("Variable %1").arg(pid));
                    
                    deps.append({
                        sourceId, "CommonEvent", sourceName,
                        targetId, "Variable", targetName,
                        "Modifies"
                    });
                }
            }
        }
    }
}
