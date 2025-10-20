#include "bgadatamanager.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

BGADataManager::BGADataManager(QObject *parent)
    : QObject(parent)
{
}

QStringList BGADataManager::getAvailableAnalyzers() const
{
    return core::availableAnalyzers();
}

QJsonArray BGADataManager::loadStringsFromGameProject(const QString &engineName, const QString &projectPath)
{
    QJsonArray extractedTextsArray;

    std::unique_ptr<core::IGameAnalyzer> analyzer = core::createAnalyzer(engineName);
    if (!analyzer) {
        emit errorOccurred(QString("Failed to create analyzer for engine: %1").arg(engineName));
        return extractedTextsArray;
    }

    core::AnalyzerOutput output = analyzer->analyze(projectPath);

    if (output.payload.isEmpty()) {
        emit errorOccurred(QString("No data extracted from project: %1").arg(projectPath));
        return extractedTextsArray;
    }

    if (output.format == "application/json") {
        QJsonDocument doc = QJsonDocument::fromJson(output.payload);
        if (doc.isNull() || !doc.isObject()) {
            emit errorOccurred("Invalid JSON output from analyzer.");
            return extractedTextsArray;
        }

        QJsonObject root = doc.object();
        extractedTextsArray = root["texts"].toArray();

    } else {
        emit errorOccurred(QString("Unsupported analyzer output format: %1").arg(output.format));
    }

    return extractedTextsArray;
}
