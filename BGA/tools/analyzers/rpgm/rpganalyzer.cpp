#include "rpganalyzer.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfoList>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

core::AnalyzerOutput RpgmAnalyzer::analyze(const QString &inputPath)
{
    QDir projectDir(inputPath);
    const QFileInfoList jsonFiles = projectDir.entryInfoList(QStringList{QStringLiteral("*.json")}, QDir::Files);

    QJsonObject root;
    root.insert(QStringLiteral("engine"), QStringLiteral("rpgm"));
    root.insert(QStringLiteral("source"), inputPath);

    QJsonArray entries;
    for (const QFileInfo &info : jsonFiles) {
        QJsonObject entry;
        entry.insert(QStringLiteral("id"), info.baseName());
        entry.insert(QStringLiteral("path"), info.absoluteFilePath());
        entry.insert(QStringLiteral("type"), QStringLiteral("json"));
        entries.append(entry);
    }

    root.insert(QStringLiteral("entries"), entries);

    QJsonDocument doc(root);

    core::AnalyzerOutput output;
    output.format = QStringLiteral("application/json");
    output.payload = doc.toJson(QJsonDocument::Indented);
    return output;
}
