#include "core/engines/rpgm/rpganalyzer.h" // Corrected include path
#include <QtCore/QDebug> // Add this include for qWarning

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QFileInfoList>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QStandardPaths> // New include

namespace core { namespace engines { namespace rpgm {

core::AnalyzerOutput RpgmAnalyzer::analyze(const QString &inputPath)
{
    QString logFilePath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/rpgm_analyzer_log.txt";
    QFile logFile(logFilePath);
    logFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream logStream(&logFile);

    logStream << "RPGM Analyzer: Starting analysis for path:" << inputPath << "\n";
    QDir projectDir(inputPath);
    logStream << "RPGM Analyzer: Getting JSON file list..." << "\n";
    const QFileInfoList jsonFiles = projectDir.entryInfoList(QStringList{QStringLiteral("*.json")}, QDir::Files);
    logStream << "RPGM Analyzer: Found" << jsonFiles.size() << "JSON files." << "\n";

    QJsonObject root;
    root.insert(QStringLiteral("engine"), QStringLiteral("rpgm"));
    root.insert(QStringLiteral("source"), inputPath);

    QJsonArray entries;
    logStream << "RPGM Analyzer: Processing JSON files..." << "\n";
    QJsonArray extractedStrings; // This will store all extracted strings

    for (const QFileInfo &info : jsonFiles) {
        // Read file content
        QFile file(info.absoluteFilePath());
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString content = in.readAll();
            file.close();

            QJsonDocument doc = QJsonDocument::fromJson(content.toUtf8());
            if (!doc.isNull()) {
                extractStringsFromJsonValue(QJsonValue::fromVariant(doc.toVariant()), extractedStrings, info.absoluteFilePath(), ""); // Pass empty string as initial key path
            } else {
                logStream << "RPGM Analyzer: Failed to parse JSON from file: " << info.absoluteFilePath() << "\\n";
            }
        } else {
            logStream << "RPGM Analyzer: Failed to open file for reading: " << info.absoluteFilePath() << "\\n";
        }
    }
    logStream << "RPGM Analyzer: Finished processing JSON files. Total extracted strings:" << extractedStrings.size() << "\\n";

    QJsonDocument doc(extractedStrings); // Create a JSON document from the array of extracted strings
    logStream << "RPGM Analyzer: Converting to JSON document...";

    core::AnalyzerOutput output;
    output.format = QStringLiteral("application/json");
    output.payload = doc.toJson(QJsonDocument::Indented);
    logStream << "RPGM Analyzer: Finished converting to JSON document. Payload size:" << output.payload.size() << "\\n";
    logFile.close();
    return output;
}

bool RpgmAnalyzer::save(const QString &outputPath, const QJsonArray &texts)
{
    QMap<QString, QJsonDocument> filesToUpdate;

    // Group texts by file path and update their content
    for (const QJsonValue &value : texts) {
        QJsonObject textObject = value.toObject();
        QString filePath = textObject["path"].toString(); // Use "path" from extracted data
        QString keyPath = textObject["key"].toString(); // Use "key" from extracted data
        QString translatedText = textObject["text"].toString(); // Use "text" which contains the translated text

        if (filePath.isEmpty() || keyPath.isEmpty()) {
            qWarning() << "Invalid data for saving: filePath or keyPath is empty.";
            continue; // Skip this entry
        }

        if (!filesToUpdate.contains(filePath)) {
            QFile file(filePath);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                qWarning() << "Failed to open file for reading:" << filePath;
                return false;
            }
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
            file.close();
            if (doc.isNull()) {
                qWarning() << "Failed to parse JSON from file:" << filePath;
                return false;
            }
            filesToUpdate.insert(filePath, doc);
        }

        QJsonDocument currentDoc = filesToUpdate.value(filePath);

        // Update the JSON value at the specified keyPath
        if (currentDoc.isObject()) {
            QJsonObject obj = currentDoc.object();
            obj[keyPath] = translatedText;
            currentDoc.setObject(obj);
        } else if (currentDoc.isArray()) {
            QJsonArray arr = currentDoc.array();
            // Check if keyPath is an array index (e.g., "0", "1")
            bool ok;
            int index = keyPath.toInt(&ok);
            if (ok && index >= 0 && index < arr.size()) {
                // If it's a direct string in the array
                if (arr.at(index).isString()) {
                    arr.replace(index, translatedText);
                    currentDoc.setArray(arr);
                } else if (arr.at(index).isObject()) {
                    // If it's an object in the array, and keyPath is just the index,
                    // we can't update a specific key within that object without more info.
                    // This case needs more complex keyPath parsing (e.g., "0.name")
                    qWarning() << "Unsupported update for object within array at index" << index << "in file:" << filePath;
                } else {
                    qWarning() << "Unsupported array element type for updating at index" << index << "in file:" << filePath;
                }
            } else {
                qWarning() << "Invalid array index or keyPath for updating in file:" << filePath;
            }
        } else {
            qWarning() << "Unsupported JSON document type for updating (not an object or array):" << filePath;
        }

        filesToUpdate.insert(filePath, currentDoc);
    }

    // Write updated content back to files
    for (const QString &filePath : filesToUpdate.keys()) {
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            qWarning() << "Failed to open file for writing:" << filePath;
            return false;
        }
        file.write(filesToUpdate.value(filePath).toJson(QJsonDocument::Indented));
        file.close();
    }

    return true;
}

void RpgmAnalyzer::extractStringsFromJsonValue(const QJsonValue &jsonValue, QJsonArray &extractedStrings, const QString &filePath, const QString &currentKeyPath)
{
    if (jsonValue.isString()) {
        QString text = jsonValue.toString();
        if (!text.isEmpty()) {
            QJsonObject entry;
            entry.insert(QStringLiteral("source"), text);
            entry.insert(QStringLiteral("path"), filePath);
            entry.insert(QStringLiteral("key"), currentKeyPath); // Store the key path
            extractedStrings.append(entry);
        }
    } else if (jsonValue.isObject()) {
        QJsonObject obj = jsonValue.toObject();
        for (const QString &key : obj.keys()) {
            QString newKeyPath = currentKeyPath.isEmpty() ? key : currentKeyPath + "." + key;
            extractStringsFromJsonValue(obj[key], extractedStrings, filePath, newKeyPath);
        }
    } else if (jsonValue.isArray()) {
        QJsonArray arr = jsonValue.toArray();
        for (int i = 0; i < arr.size(); ++i) {
            QString newKeyPath = currentKeyPath.isEmpty() ? QString::number(i) : currentKeyPath + "[" + QString::number(i) + "]";
            extractStringsFromJsonValue(arr[i], extractedStrings, filePath, newKeyPath);
        }
    }
}

} // namespace rpgm
} // namespace engines
} // namespace core
