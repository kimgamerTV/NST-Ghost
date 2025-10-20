#include "unityanalyzer.h"

#include <QtCore/QDirIterator>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

core::AnalyzerOutput UnityAnalyzer::analyze(const QString &inputPath)
{
    QJsonObject root;
    root.insert(QStringLiteral("engine"), QStringLiteral("unity"));
    root.insert(QStringLiteral("source"), inputPath);

    QJsonArray entries;
    QDirIterator iterator(inputPath, QStringList{QStringLiteral("*.asset"), QStringLiteral("*.prefab")},
                          QDir::Files, QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
        const QString path = iterator.next();
        QJsonObject entry;
        entry.insert(QStringLiteral("path"), path);
        entry.insert(QStringLiteral("type"), iterator.fileInfo().suffix());
        entries.append(entry);
    }

    root.insert(QStringLiteral("entries"), entries);

    QJsonDocument doc(root);

    core::AnalyzerOutput output;
    output.format = QStringLiteral("application/json");
    output.payload = doc.toJson(QJsonDocument::Indented);
    return output;
}
