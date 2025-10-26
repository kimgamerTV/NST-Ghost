#include "bgadatamanager.h"
#include <QThread> // Required for QThread::currentThreadId()
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile> // New include
#include <QTextStream> // New include
#include <QStandardPaths> // New include

BGADataManager::BGADataManager(QObject *parent)
    : QObject(parent)
{
}

QJsonArray BGADataManager::loadedFonts() const
{
    return m_loadedFonts;
}

QStringList BGADataManager::getAvailableAnalyzers() const
{
    return core::availableAnalyzers();
}

QJsonArray BGADataManager::loadStringsFromGameProject(const QString &engineName, const QString &projectPath)
{
    qDebug() << "BGADataManager: loadStringsFromGameProject called in thread:" << QThread::currentThreadId();
    emit progressUpdated(0, "Starting project analysis...");

    QString logFilePath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/bgadatamanager_log.txt";
    QFile logFile(logFilePath);
    logFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream logStream(&logFile);

    QJsonArray extractedTextsArray;

    logStream << "BGADataManager: Creating analyzer for engine: " << engineName << "\n";
    std::unique_ptr<core::IGameAnalyzer> analyzer = core::createAnalyzer(engineName);
    if (!analyzer) {
        emit errorOccurred(QString("Failed to create analyzer for engine: %1").arg(engineName));
        logStream << "BGADataManager: Failed to create analyzer." << "\n";
        logFile.close();
        emit loadingFinished();
        return extractedTextsArray;
    }

    emit progressUpdated(25, "Analyzing game project...");
    logStream << "BGADataManager: Calling analyzer->analyze for project: " << projectPath << "\n";
    core::AnalyzerOutput output = analyzer->analyze(projectPath);
    logStream << "BGADataManager: Analyzer output payload size:" << output.payload.size() << "bytes" << "\n";

    if (!output.errorMessage.isEmpty()) { // Check for error message from analyzer
        emit errorOccurred(output.errorMessage);
        logStream << "BGADataManager: Analyzer returned error: " << output.errorMessage << "\n";
        logFile.close();
        emit loadingFinished();
        return extractedTextsArray; // Return empty array on error
    }

    if (output.payload.isEmpty()) {
        emit errorOccurred(QString("No data extracted from project: %1").arg(projectPath));
        logStream << "BGADataManager: No data extracted from project." << "\n";
        logFile.close();
        emit loadingFinished();
        return extractedTextsArray;
    }

    emit progressUpdated(75, "Parsing extracted data...");
    logStream << "BGADataManager: Before QJsonDocument::fromJson" << "\n";
    if (output.format == "application/json") {
        QJsonDocument doc = QJsonDocument::fromJson(output.payload);
        logStream << "BGADataManager: After QJsonDocument::fromJson" << "\n";
        if (doc.isNull()) {
            emit errorOccurred("Invalid JSON output from analyzer.");
            logStream << "BGADataManager: Invalid JSON output from analyzer." << "\n";
            logFile.close();
            emit loadingFinished();
            return extractedTextsArray;
        }

        if (doc.isObject()) {
            QJsonObject rootObj = doc.object();
            if (rootObj.contains("strings") && rootObj["strings"].isArray()) {
                extractedTextsArray = rootObj["strings"].toArray();
            }
            if (rootObj.contains("fonts") && rootObj["fonts"].isArray()) {
                m_loadedFonts = rootObj["fonts"].toArray();
                emit fontsLoaded(m_loadedFonts);
            }
        } else if (doc.isArray()) {
            extractedTextsArray = doc.array();
        }

        emit progressUpdated(90, QString("Extracted %1 entries.").arg(extractedTextsArray.size()));
        logStream << "BGADataManager: Extracted " << extractedTextsArray.size() << " entries." << "\n";
        logStream << "BGADataManager: After extracting entries" << "\n";

    } else {
        logStream << "BGADataManager: Unsupported format detected: " << output.format << "\n";
        emit errorOccurred(QString("Unsupported analyzer output format: %1").arg(output.format));
        logStream << "BGADataManager: Unsupported analyzer output format." << "\n";
        logFile.close();
        emit loadingFinished();
        return extractedTextsArray;
    }
    logStream << "BGADataManager: loadStringsFromGameProject returning." << "\n";
    logFile.close();
    emit loadingFinished();
    qDebug() << "BGADataManager: loadStringsFromGameProject finished in thread:" << QThread::currentThreadId();
    return extractedTextsArray;
}
    
bool BGADataManager::saveStringsToGameProject(const QString &engineName, const QString &projectPath, const QMap<QString, QJsonArray> &data)
{
    std::unique_ptr<core::IGameAnalyzer> analyzer = core::createAnalyzer(engineName);
    if (!analyzer) {
        emit errorOccurred(QString("Failed to create analyzer for engine: %1").arg(engineName));
        return false;
    }

    // Convert the QMap<QString, QJsonArray> to a single QJsonArray suitable for the analyzer's save method
    QJsonArray textsToSave;
    for (const QJsonArray &fileTexts : data.values()) {
        for (const QJsonValue &value : fileTexts) {
            textsToSave.append(value);
        }
    }

    if (!analyzer->save(projectPath, textsToSave)) {
        emit errorOccurred(QString("Failed to save strings to project: %1").arg(projectPath));
        return false;
    }
    return true;
}
