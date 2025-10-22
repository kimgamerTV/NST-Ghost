#include "core/engines/unity/unityanalyzer.h" // Corrected include path
#include <QtCore/QDebug> // For qWarning

#include <QtCore/QDirIterator>
#include <QtCore/QDebug> // For qWarning
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

namespace core { namespace engines { namespace unity {


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

bool UnityAnalyzer::save(const QString &outputPath, const QJsonArray &texts)
{
    Q_UNUSED(outputPath); // Suppress unused parameter warning
    Q_UNUSED(texts); // Suppress unused parameter warning
    qWarning() << "Saving for Unity projects is not yet implemented due to complex asset format.";
    return false;
}

} // namespace unity
} // namespace engines
} // namespace core
