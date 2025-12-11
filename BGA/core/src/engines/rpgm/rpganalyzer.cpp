#include "core/engines/rpgm/rpganalyzer.h"
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QFileInfoList>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QStandardPaths>
#include <QOperatingSystemVersion>
#include <QRegularExpression>

namespace core { namespace engines { namespace rpgm {

core::AnalyzerOutput RpgmAnalyzer::analyze(const QString &inputPath)
{
    QString logFilePath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/rpgm_analyzer_log.txt";
    QFile logFile(logFilePath);

    // ตรวจสอบว่าเปิดไฟล์สำเร็จหรือไม่
    if (!logFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
        qWarning() << "Failed to open log file:" << logFilePath;
    }

    QTextStream logStream(&logFile);
    logStream << "RPGM Analyzer: Starting analysis for path: " << inputPath << "\n";

    QDir projectDir(inputPath);
    QFileInfoList jsonFiles;

    // ตรวจสอบว่าเป็น project root หรือ data folder
    bool isDataFolder = projectDir.dirName().toLower() == "data";

    if (isDataFolder) {
        // ถ้าชี้ไปที่ data folder โดยตรง
        logStream << "RPGM Analyzer: Input path is data folder\n";
        jsonFiles = projectDir.entryInfoList(QStringList{QStringLiteral("*.json")}, QDir::Files);
    } else {
        // ถ้าชี้ไปที่ project root ให้หาทั้ง root และ data folder
        logStream << "RPGM Analyzer: Searching in project root\n";

        // ค้นหาในโฟลเดอร์ data (RPG Maker MV/MZ)
        QDir dataDir(projectDir.filePath("data"));
        if (dataDir.exists()) {
            logStream << "RPGM Analyzer: Found 'data' folder\n";
            jsonFiles += dataDir.entryInfoList(QStringList{QStringLiteral("*.json")}, QDir::Files);
        }

        // ค้นหาในโฟลเดอร์ Data (case-insensitive)
        QDir dataDirAlt(projectDir.filePath("Data"));
        if (dataDirAlt.exists() && dataDirAlt.path() != dataDir.path()) {
            logStream << "RPGM Analyzer: Found 'Data' folder\n";
            jsonFiles += dataDirAlt.entryInfoList(QStringList{QStringLiteral("*.json")}, QDir::Files);
        }

        // ค้นหาไฟล์ JSON ใน root (สำหรับ package.json, plugins.json ฯลฯ)
        QFileInfoList rootJsonFiles = projectDir.entryInfoList(
            QStringList{QStringLiteral("*.json")},
            QDir::Files
            );

        // กรองเฉพาะไฟล์ที่เกี่ยวข้อง
        QStringList relevantRootFiles = {
            "package.json", "plugins.js", "System.json"
        };

        for (const QFileInfo &info : rootJsonFiles) {
            if (relevantRootFiles.contains(info.fileName(), Qt::CaseInsensitive)) {
                jsonFiles.append(info);
            }
        }

        // ค้นหาใน js/plugins (สำหรับ RPG Maker MV/MZ)
        QDir pluginsDir(projectDir.filePath("js/plugins"));
        if (pluginsDir.exists()) {
            logStream << "RPGM Analyzer: Found 'js/plugins' folder\n";
            jsonFiles += pluginsDir.entryInfoList(QStringList{QStringLiteral("*.json")}, QDir::Files);
        }
    }

    logStream << "RPGM Analyzer: Found " << jsonFiles.size() << " JSON files.\n";

    // Font searching logic (แก้ไขให้ดีขึ้น)
    QDir parentDir;
    if (isDataFolder) {
        parentDir = QDir(inputPath);
        parentDir.cdUp(); // ย้อนขึ้นไปหา parent directory
    } else {
        parentDir = QDir(inputPath);
    }

    QDir fontDir(parentDir.filePath("fonts"));
    if (!fontDir.exists()) {
        fontDir.setPath(parentDir.filePath("Fonts"));
    }

    QJsonArray fontEntries;
    if (fontDir.exists()) {
        logStream << "RPGM Analyzer: Found fonts folder at: " << fontDir.path() << "\n";
        const QFileInfoList fontFiles = fontDir.entryInfoList(
            QStringList() << "*.ttf" << "*.otf" << "*.woff" << "*.woff2",
            QDir::Files
            );
        for (const QFileInfo &info : fontFiles) {
            QJsonObject fontEntry;
            fontEntry.insert(QStringLiteral("name"), info.baseName());
            QString fontFilePath = info.absoluteFilePath();
#ifdef Q_OS_WIN
            fontFilePath.replace(QStringLiteral("/"), QStringLiteral("\\"));
#endif
            fontEntry.insert(QStringLiteral("path"), fontFilePath);
            fontEntries.append(fontEntry);
        }
        logStream << "RPGM Analyzer: Found " << fontFiles.size() << " font files.\n";
    } else {
        logStream << "RPGM Analyzer: No fonts folder found\n";
    }

    QJsonArray extractedStrings;
    logStream << "RPGM Analyzer: Processing JSON files...\n";

    int processedCount = 0;
    int failedCount = 0;

    for (const QFileInfo &info : jsonFiles) {
        qDebug() << "RpgmAnalyzer: Processing file:" << info.absoluteFilePath();
        // Skip package.json as it's usually not for translation
        if (info.fileName().toLower() == "package.json") {
            qDebug() << "RpgmAnalyzer: Skipping package.json";
            continue;
        }

        QFile file(info.absoluteFilePath());
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QByteArray content = file.readAll();
            file.close();

            QJsonParseError parseError;
            QJsonDocument doc = QJsonDocument::fromJson(content, &parseError);

            if (!doc.isNull()) {
                int prevCount = extractedStrings.size();
                extractStringsFromJsonValue(
                    QJsonValue::fromVariant(doc.toVariant()),
                    extractedStrings,
                    info.absoluteFilePath(),
                    ""
                    );
                qDebug() << "RpgmAnalyzer: Processed" << info.fileName() << "Extracted" << (extractedStrings.size() - prevCount) << "strings.";
                processedCount++;
            } else {
                logStream << "RPGM Analyzer: Failed to parse JSON from file: "
                          << info.absoluteFilePath()
                          << " Error: " << parseError.errorString() << "\n";
                qWarning() << "RpgmAnalyzer: Failed to parse JSON from file:" << info.absoluteFilePath() << "Error:" << parseError.errorString();
                failedCount++;
            }
        } else {
            logStream << "RPGM Analyzer: Failed to open file for reading: "
                      << info.absoluteFilePath() << "\n";
            failedCount++;
        }
    }

    logStream << "RPGM Analyzer: Processing complete.\n";
    logStream << "  - Processed: " << processedCount << " files\n";
    logStream << "  - Failed: " << failedCount << " files\n";
    logStream << "  - Total extracted strings: " << extractedStrings.size() << "\n";

    QJsonObject rootObject;
    rootObject.insert(QStringLiteral("strings"), extractedStrings);
    rootObject.insert(QStringLiteral("fonts"), fontEntries);
    rootObject.insert(QStringLiteral("engine"), QStringLiteral("rpgm"));
    rootObject.insert(QStringLiteral("source"), inputPath);
    rootObject.insert(QStringLiteral("filesProcessed"), processedCount);
    rootObject.insert(QStringLiteral("filesFailed"), failedCount);

    QJsonDocument doc(rootObject);

    core::AnalyzerOutput output;
    output.format = QStringLiteral("application/json");
    output.payload = doc.toJson(QJsonDocument::Indented);

    logStream << "RPGM Analyzer: Analysis complete. Payload size: "
              << output.payload.size() << " bytes\n";
    logFile.close();

    return output;
}

bool RpgmAnalyzer::save(const QString &outputPath, const QJsonArray &texts)
{
    // จัดกลุ่มข้อมูลตาม file path เพื่อลด I/O operations
    QMap<QString, QVector<QPair<QString, QString>>> fileUpdates; // filePath -> [(keyPath, newValue)]

    for (const QJsonValue &value : texts) {
        QJsonObject textObject = value.toObject();
        QString filePath = textObject["path"].toString();
        QString keyPath = textObject["key"].toString();
        QString translatedText = textObject["text"].toString();

        if (filePath.isEmpty() || keyPath.isEmpty()) {
            qWarning() << "Invalid data for saving: filePath or keyPath is empty.";
            continue;
        }

        fileUpdates[filePath].append(qMakePair(keyPath, translatedText));
    }

    // อ่านและอัพเดทแต่ละไฟล์
    QMap<QString, QJsonDocument> filesToUpdate;

    for (auto it = fileUpdates.constBegin(); it != fileUpdates.constEnd(); ++it) {
        const QString &filePath = it.key();

        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "Failed to open file for reading:" << filePath;
            return false;
        }

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
        file.close();

        if (doc.isNull()) {
            qWarning() << "Failed to parse JSON from file:" << filePath
                       << "Error:" << parseError.errorString();
            return false;
        }

        // อัพเดทค่าทั้งหมดสำหรับไฟล์นี้
        for (const auto &update : it.value()) {
            const QString &keyPath = update.first;
            const QString &newValue = update.second;

            if (!updateJsonValue(doc, keyPath, newValue)) {
                qWarning() << "Failed to update value at keyPath:" << keyPath
                           << "in file:" << filePath;
            }
        }

        filesToUpdate.insert(filePath, doc);
    }

    // เขียนกลับไปยังไฟล์ทั้งหมด (transaction-like approach)
    for (auto it = filesToUpdate.constBegin(); it != filesToUpdate.constEnd(); ++it) {
        const QString &filePath = it.key();
        const QJsonDocument &doc = it.value();

        // สร้าง backup ก่อน (optional)
        QString backupPath = filePath + ".backup";
        QFile::remove(backupPath); // ลบ backup เก่าถ้ามี
        QFile::copy(filePath, backupPath);

        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            qWarning() << "Failed to open file for writing:" << filePath;
            // พยายาม restore จาก backup
            QFile::remove(filePath);
            QFile::copy(backupPath, filePath);
            return false;
        }

        QByteArray jsonData = doc.toJson(QJsonDocument::Indented);
        if (file.write(jsonData) != jsonData.size()) {
            qWarning() << "Failed to write complete data to file:" << filePath;
            file.close();
            // พยายาม restore จาก backup
            QFile::remove(filePath);
            QFile::copy(backupPath, filePath);
            return false;
        }

        file.close();

        // ลบ backup เมื่อสำเร็จ
        QFile::remove(backupPath);
    }

    return true;
}

bool RpgmAnalyzer::updateJsonValue(QJsonDocument &doc, const QString &keyPath, const QString &newValue)
{
    QStringList keys = keyPath.split('.');

    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        if (updateJsonObject(obj, keys, 0, newValue)) {
            doc.setObject(obj);
            return true;
        }
    } else if (doc.isArray()) {
        QJsonArray arr = doc.array();
        if (updateJsonArray(arr, keys, 0, newValue)) {
            doc.setArray(arr);
            return true;
        }
    }

    return false;
}

bool RpgmAnalyzer::updateJsonObject(QJsonObject &obj, const QStringList &keys, int index, const QString &newValue)
{
    if (index >= keys.size()) return false;

    QString currentKey = keys[index];

    // Check if key contains array notation [n] (possibly multiple like [0][0])
    if (currentKey.contains('[')) {
        int bracketPos = currentKey.indexOf('[');
        QString baseKey = currentKey.left(bracketPos);
        
        // Extract all array indices from the key (e.g., "parameters[0][0]" -> [0, 0])
        QList<int> arrayIndices;
        QString remaining = currentKey.mid(bracketPos);
        QRegularExpression re("\\[(\\d+)\\]");
        QRegularExpressionMatchIterator it = re.globalMatch(remaining);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            arrayIndices.append(match.captured(1).toInt());
        }
        
        if (arrayIndices.isEmpty()) return false;
        if (!obj.contains(baseKey)) return false;
        
        // Navigate through the nested arrays
        QJsonValue currentVal = obj[baseKey];
        QList<QPair<QJsonArray, int>> pathStack; // Stack to track path for reconstruction
        
        for (int i = 0; i < arrayIndices.size(); ++i) {
            int idx = arrayIndices[i];
            
            if (!currentVal.isArray()) return false;
            QJsonArray arr = currentVal.toArray();
            if (idx < 0 || idx >= arr.size()) return false;
            
            pathStack.append(qMakePair(arr, idx));
            currentVal = arr[idx];
        }
        
        // Now currentVal is the value we need to update or navigate into
        if (index == keys.size() - 1) {
            // This is the last key, replace the value
            // Reconstruct the path backwards
            QJsonValue newVal = QJsonValue(newValue);
            for (int i = pathStack.size() - 1; i >= 0; --i) {
                QJsonArray arr = pathStack[i].first;
                int idx = pathStack[i].second;
                arr.replace(idx, newVal);
                newVal = arr;
            }
            obj[baseKey] = newVal.toArray();
            return true;
        } else {
            // Navigate into the element
            if (currentVal.isObject()) {
                QJsonObject elementObj = currentVal.toObject();
                if (updateJsonObject(elementObj, keys, index + 1, newValue)) {
                    // Reconstruct the path backwards
                    QJsonValue newVal = elementObj;
                    for (int i = pathStack.size() - 1; i >= 0; --i) {
                        QJsonArray arr = pathStack[i].first;
                        int idx = pathStack[i].second;
                        arr.replace(idx, newVal);
                        newVal = arr;
                    }
                    obj[baseKey] = newVal.toArray();
                    return true;
                }
            } else if (currentVal.isArray()) {
                QJsonArray elementArr = currentVal.toArray();
                if (updateJsonArray(elementArr, keys, index + 1, newValue)) {
                    // Reconstruct the path backwards
                    QJsonValue newVal = elementArr;
                    for (int i = pathStack.size() - 1; i >= 0; --i) {
                        QJsonArray arr = pathStack[i].first;
                        int idx = pathStack[i].second;
                        arr.replace(idx, newVal);
                        newVal = arr;
                    }
                    obj[baseKey] = newVal.toArray();
                    return true;
                }
            }
        }
    } else {
        if (!obj.contains(currentKey)) return false;

        if (index == keys.size() - 1) {
            obj[currentKey] = newValue;
            return true;
        } else {
            QJsonValue value = obj[currentKey];
            if (value.isObject()) {
                QJsonObject nestedObj = value.toObject();
                if (updateJsonObject(nestedObj, keys, index + 1, newValue)) {
                    obj[currentKey] = nestedObj;
                    return true;
                }
            } else if (value.isArray()) {
                QJsonArray nestedArr = value.toArray();
                if (updateJsonArray(nestedArr, keys, index + 1, newValue)) {
                    obj[currentKey] = nestedArr;
                    return true;
                }
            }
        }
    }

    return false;
}

bool RpgmAnalyzer::updateJsonArray(QJsonArray &arr, const QStringList &keys, int index, const QString &newValue)
{
    if (index >= keys.size()) return false;

    QString currentKey = keys[index];

    // Check if it's an array index (possibly multiple like [0][0])
    if (currentKey.contains('[')) {
        // Extract all array indices from the key
        QList<int> arrayIndices;
        QRegularExpression re("\\[(\\d+)\\]");
        QRegularExpressionMatchIterator it = re.globalMatch(currentKey);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            arrayIndices.append(match.captured(1).toInt());
        }
        
        if (arrayIndices.isEmpty()) return false;
        
        // Navigate through the nested arrays
        QJsonValue currentVal = arr[arrayIndices[0]];
        if (arrayIndices[0] < 0 || arrayIndices[0] >= arr.size()) return false;
        
        QList<QPair<QJsonArray, int>> pathStack;
        pathStack.append(qMakePair(arr, arrayIndices[0]));
        
        for (int i = 1; i < arrayIndices.size(); ++i) {
            int idx = arrayIndices[i];
            
            if (!currentVal.isArray()) return false;
            QJsonArray nestedArr = currentVal.toArray();
            if (idx < 0 || idx >= nestedArr.size()) return false;
            
            pathStack.append(qMakePair(nestedArr, idx));
            currentVal = nestedArr[idx];
        }
        
        if (index == keys.size() - 1) {
            // This is the last key, replace the value
            QJsonValue newVal = QJsonValue(newValue);
            for (int i = pathStack.size() - 1; i >= 0; --i) {
                QJsonArray stackArr = pathStack[i].first;
                int idx = pathStack[i].second;
                stackArr.replace(idx, newVal);
                newVal = stackArr;
            }
            // Copy back to arr (first level)
            QJsonArray resultArr = newVal.toArray();
            for (int i = 0; i < resultArr.size(); ++i) {
                arr.replace(i, resultArr[i]);
            }
            return true;
        } else {
            QJsonValue value = currentVal;
            if (value.isObject()) {
                QJsonObject obj = value.toObject();
                if (updateJsonObject(obj, keys, index + 1, newValue)) {
                    QJsonValue newVal = obj;
                    for (int i = pathStack.size() - 1; i >= 0; --i) {
                        QJsonArray stackArr = pathStack[i].first;
                        int idx = pathStack[i].second;
                        stackArr.replace(idx, newVal);
                        newVal = stackArr;
                    }
                    QJsonArray resultArr = newVal.toArray();
                    for (int i = 0; i < resultArr.size(); ++i) {
                        arr.replace(i, resultArr[i]);
                    }
                    return true;
                }
            } else if (value.isArray()) {
                QJsonArray nestedArr = value.toArray();
                if (updateJsonArray(nestedArr, keys, index + 1, newValue)) {
                    QJsonValue newVal = nestedArr;
                    for (int i = pathStack.size() - 1; i >= 0; --i) {
                        QJsonArray stackArr = pathStack[i].first;
                        int idx = pathStack[i].second;
                        stackArr.replace(idx, newVal);
                        newVal = stackArr;
                    }
                    QJsonArray resultArr = newVal.toArray();
                    for (int i = 0; i < resultArr.size(); ++i) {
                        arr.replace(i, resultArr[i]);
                    }
                    return true;
                }
            }
        }
    } else {
        bool ok;
        int arrayIndex = currentKey.toInt(&ok);

        if (ok && arrayIndex >= 0 && arrayIndex < arr.size()) {
            if (index == keys.size() - 1) {
                arr.replace(arrayIndex, newValue);
                return true;
            } else {
                QJsonValue value = arr[arrayIndex];
                if (value.isObject()) {
                    QJsonObject obj = value.toObject();
                    if (updateJsonObject(obj, keys, index + 1, newValue)) {
                        arr.replace(arrayIndex, obj);
                        return true;
                    }
                } else if (value.isArray()) {
                    QJsonArray nestedArr = value.toArray();
                    if (updateJsonArray(nestedArr, keys, index + 1, newValue)) {
                        arr.replace(arrayIndex, nestedArr);
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

void RpgmAnalyzer::extractStringsFromJsonValue(const QJsonValue &jsonValue, QJsonArray &extractedStrings, const QString &filePath, const QString &currentKeyPath)
{
    if (jsonValue.isString()) {
        QString text = jsonValue.toString();
        
        if (!text.isEmpty() && !isSystemString(text)) {
            QJsonObject entry;
            entry.insert(QStringLiteral("source"), text);
            entry.insert(QStringLiteral("path"), filePath);
            entry.insert(QStringLiteral("key"), currentKeyPath);
            extractedStrings.append(entry);
        }
    } else if (jsonValue.isObject()) {
        QJsonObject obj = jsonValue.toObject();

        // ===== Rule 1: Event Command Handling (Complete) =====
        if (obj.contains("code") && obj.contains("parameters") && obj["parameters"].isArray()) {
            int code = obj["code"].toInt();
            QJsonArray params = obj["parameters"].toArray();
            
            bool extractedFromCommand = false;

            switch (code) {
                // === Text Display Commands ===
                case 101: // Show Text: Character Name/Face Setup
                    // parameters[4] = actor name (TRANSLATABLE!)
                    if (params.size() > 4 && params[4].isString()) {
                        QString actorName = params[4].toString();
                        QString newKeyPath = currentKeyPath.isEmpty() ? "parameters[4]" : currentKeyPath + ".parameters[4]";
                        
                        if (!actorName.isEmpty() && !isSystemString(actorName)) {
                            QJsonObject entry;
                            entry.insert(QStringLiteral("source"), actorName);
                            entry.insert(QStringLiteral("path"), filePath);
                            entry.insert(QStringLiteral("key"), newKeyPath);
                            extractedStrings.append(entry);
                            extractedFromCommand = true;
                        }
                    }
                    break;
                    
                case 401: // Show Text: Dialogue Line
                    if (params.size() > 0 && params[0].isString()) {
                        QString text = params[0].toString();
                        QString newKeyPath = currentKeyPath.isEmpty() ? "parameters[0]" : currentKeyPath + ".parameters[0]";
                        
                        if (!text.isEmpty() && !isSystemString(text)) {
                            QJsonObject entry;
                            entry.insert(QStringLiteral("source"), text);
                            entry.insert(QStringLiteral("path"), filePath);
                            entry.insert(QStringLiteral("key"), newKeyPath);
                            extractedStrings.append(entry);
                            extractedFromCommand = true;
                        }
                    }
                    break;
                
                case 102: // Show Choices
                    // parameters[0] = array of choice texts
                    if (params.size() > 0 && params[0].isArray()) {
                        QJsonArray choices = params[0].toArray();
                        for (int i = 0; i < choices.size(); ++i) {
                            if (choices[i].isString()) {
                                QString choiceText = choices[i].toString();
                                QString newKeyPath = currentKeyPath.isEmpty() 
                                    ? QString("parameters[0][%1]").arg(i)
                                    : QString("%1.parameters[0][%2]").arg(currentKeyPath).arg(i);
                                
                                if (!choiceText.isEmpty() && !isSystemString(choiceText)) {
                                    QJsonObject entry;
                                    entry.insert(QStringLiteral("source"), choiceText);
                                    entry.insert(QStringLiteral("path"), filePath);
                                    entry.insert(QStringLiteral("key"), newKeyPath);
                                    extractedStrings.append(entry);
                                }
                            }
                        }
                        extractedFromCommand = true;
                    }
                    break;
                
                case 103: // Input Number
                    // No translatable text
                    break;
                
                case 104: // Select Item
                    // parameters[1] = item type (system value)
                    break;
                
                case 105: // Show Scrolling Text
                    // Similar to 401 but for scrolling
                    if (params.size() > 0 && params[0].isString()) {
                        QString text = params[0].toString();
                        QString newKeyPath = currentKeyPath.isEmpty() ? "parameters[0]" : currentKeyPath + ".parameters[0]";
                        
                        if (!text.isEmpty() && !isSystemString(text)) {
                            QJsonObject entry;
                            entry.insert(QStringLiteral("source"), text);
                            entry.insert(QStringLiteral("path"), filePath);
                            entry.insert(QStringLiteral("key"), newKeyPath);
                            extractedStrings.append(entry);
                            extractedFromCommand = true;
                        }
                    }
                    break;
                
                case 405: // Show Scrolling Text continuation
                    if (params.size() > 0 && params[0].isString()) {
                        QString text = params[0].toString();
                        QString newKeyPath = currentKeyPath.isEmpty() ? "parameters[0]" : currentKeyPath + ".parameters[0]";
                        
                        if (!text.isEmpty() && !isSystemString(text)) {
                            QJsonObject entry;
                            entry.insert(QStringLiteral("source"), text);
                            entry.insert(QStringLiteral("path"), filePath);
                            entry.insert(QStringLiteral("key"), newKeyPath);
                            extractedStrings.append(entry);
                            extractedFromCommand = true;
                        }
                    }
                    break;
                
                // === System Commands (Usually No Translation) ===
                case 108: // Comment
                case 408: // Comment continuation
                    // Developer comments - usually skip, but could add option to extract
                    extractedFromCommand = true; // Fix: Prevent recursion
                    break;
                
                case 111: // Conditional Branch
                case 112: // Loop
                case 113: // Break Loop
                case 115: // Exit Event Processing
                case 117: // Common Event
                case 118: // Label
                case 119: // Jump to Label
                case 121: // Control Switches
                case 122: // Control Variables
                case 123: // Control Self Switch
                case 124: // Control Timer
                    // Pure logic - no translatable content
                    extractedFromCommand = true;
                    break;
                
                // === Game Progression Commands ===
                case 125: // Change Gold
                case 126: // Change Items
                case 127: // Change Weapons
                case 128: // Change Armors
                case 129: // Change Party Member
                    // System commands - no text
                    extractedFromCommand = true;
                    break;
                
                // === Screen Commands ===
                case 201: // Transfer Player
                case 202: // Set Vehicle Location
                case 203: // Set Event Location
                case 204: // Scroll Map
                case 205: // Set Movement Route
                    // Map/movement commands - no text
                    extractedFromCommand = true;
                    break;
                
                case 231: // Show Picture
                    // parameters[1] = picture name (filename - skip)
                    extractedFromCommand = true; // Fix: Prevent recursion so "PictureZIndex" isn't extracted
                    break;
                
                case 241: // Play BGM
                case 242: // Fadeout BGM
                case 245: // Play BGS
                case 246: // Fadeout BGS
                case 249: // Play ME
                case 250: // Play SE
                    // Audio commands - filenames only (parameters contain {name, volume, pitch, pan})
                    // Mark as handled to prevent recursing into parameters which would incorrectly extract 'name'
                    extractedFromCommand = true;
                    break;
                
                // === Battle Commands ===
                case 301: // Battle Processing
                case 311: // Change HP
                case 312: // Change MP
                case 313: // Change State
                case 314: // Recover All
                case 339: // Force Action
                    // Battle system commands - no text
                    extractedFromCommand = true;
                    break;
                
                // === Actor Commands ===
                case 320: // Change Name
                    // parameters[1] = new actor name (TRANSLATABLE!)
                    if (params.size() > 1 && params[1].isString()) {
                        QString actorName = params[1].toString();
                        QString newKeyPath = currentKeyPath.isEmpty() ? "parameters[1]" : currentKeyPath + ".parameters[1]";
                        
                        if (!actorName.isEmpty() && !isSystemString(actorName)) {
                            QJsonObject entry;
                            entry.insert(QStringLiteral("source"), actorName);
                            entry.insert(QStringLiteral("path"), filePath);
                            entry.insert(QStringLiteral("key"), newKeyPath);
                            extractedStrings.append(entry);
                            extractedFromCommand = true;
                        }
                    }
                    break;
                
                case 324: // Change Nickname
                    // parameters[1] = new nickname (TRANSLATABLE!)
                    if (params.size() > 1 && params[1].isString()) {
                        QString nickname = params[1].toString();
                        QString newKeyPath = currentKeyPath.isEmpty() ? "parameters[1]" : currentKeyPath + ".parameters[1]";
                        
                        if (!nickname.isEmpty() && !isSystemString(nickname)) {
                            QJsonObject entry;
                            entry.insert(QStringLiteral("source"), nickname);
                            entry.insert(QStringLiteral("path"), filePath);
                            entry.insert(QStringLiteral("key"), newKeyPath);
                            extractedStrings.append(entry);
                            extractedFromCommand = true;
                        }
                    }
                    break;
                
                // === Plugin Commands ===
                case 356: // Plugin Command (MV)
                case 357: // Plugin Command (MZ)
                    // parameters[0] = plugin command string
                    // Usually technical (like "EnemyBook open"), but some plugins use translatable text
                    // Best to skip unless specifically needed
                    // Add to blacklist or use heuristic
                    extractedFromCommand = true; // Fix: Prevent recursion
                    break;
                
                // === Script Commands ===
                case 355: // Script
                case 655: // Script continuation
                    // JavaScript code - never translate
                    extractedFromCommand = true; // Fix: Prevent recursion
                    break;
                
                default:
                    // Unknown command - log but don't extract
                    qDebug() << "RpgmAnalyzer: Unknown event code:" << code 
                             << "in file:" << filePath;
                    break;
            }
            
            // Recurse into nested structures (like lists within events)
            // But skip parameters array if we already extracted from it
            for (const QString &key : obj.keys()) {
                if (key == "parameters" && extractedFromCommand) {
                    continue; // Already handled above
                }
                
                // Skip technical keys
                static const QSet<QString> technicalKeys = {
                    "code", "indent"
                };
                if (technicalKeys.contains(key)) {
                    continue;
                }
                
                QJsonValue val = obj[key];
                QString newKeyPath = currentKeyPath.isEmpty() ? key : currentKeyPath + "." + key;
                
                if (val.isArray() || val.isObject()) {
                    extractStringsFromJsonValue(val, extractedStrings, filePath, newKeyPath);
                }
            }
            
            return; // Done with event command
        }

        // ===== Rule 2: Database Objects - Strict Whitelist =====
        static const QSet<QString> whitelistedKeys = {
            "name",
            "description", 
            "message1",
            "message2",
            "message3",
            "message4",
            "note",
            "nickname",
            "profile",
            // System.json additions
            "gameTitle",
            "currencyUnit",
            // Terms and messages
            "terms",
            "basic",
            "commands",
            "params",
            "messages",
            // Specific message keys
            "actionFailure", "actorDamage", "actorDrain", "actorGain", "actorLoss", 
            "actorNoDamage", "actorNoHit", "actorRecovery", "alwaysDash", "bgmVolume", 
            "bgsVolume", "buffAdd", "buffRemove", "commandRemember", "counterAttack", 
            "criticalToActor", "criticalToEnemy", "debuffAdd", "defeat", "emerge", 
            "enemyDamage", "enemyDrain", "enemyGain", "enemyLoss", "enemyNoDamage", 
            "enemyNoHit", "enemyRecovery", "escapeFailure", "escapeStart", "evasion", 
            "expNext", "expTotal", "file", "levelUp", "loadMessage", "magicEvasion", 
            "magicReflection", "meVolume", "obtainExp", "obtainGold", "obtainItem", 
            "obtainSkill", "partyName", "possession", "preemptive", "saveMessage", 
            "seVolume", "substitute", "surprise", "useItem", "victory"
        };
        
        // Keys to always ignore (contain filenames/technical data)
        static const QSet<QString> blacklistedKeys = {
            "se", "bgm", "bgs", "me", 
            "animation1Name", "animation2Name",
            "battlerName", "characterName", "faceName",
            "motion", "overlay1Name", "overlay2Name",
            "tileset", "parallaxName", "battleback1Name", "battleback2Name",
            "script", "url"
        };
        
        for (const QString &key : obj.keys()) {
            // Skip blacklisted keys entirely
            if (blacklistedKeys.contains(key)) {
                continue;
            }
            
            QJsonValue val = obj[key];
            QString newKeyPath = currentKeyPath.isEmpty() ? key : currentKeyPath + "." + key;
            
            if (val.isString()) {
                // Only extract if key is whitelisted
                if (whitelistedKeys.contains(key)) {
                    extractStringsFromJsonValue(val, extractedStrings, filePath, newKeyPath);
                }
            } else if (val.isArray() || val.isObject()) {
                // Recurse to find nested whitelisted keys
                extractStringsFromJsonValue(val, extractedStrings, filePath, newKeyPath);
            }
        }
    } else if (jsonValue.isArray()) {
        QJsonArray arr = jsonValue.toArray();
        for (int i = 0; i < arr.size(); ++i) {
            QString newKeyPath = currentKeyPath.isEmpty()
                ? QString::number(i)
                : currentKeyPath + "[" + QString::number(i) + "]";
            extractStringsFromJsonValue(arr[i], extractedStrings, filePath, newKeyPath);
        }
    }
}

bool RpgmAnalyzer::isSystemString(const QString &text)
{
    // Empty or whitespace only
    if (text.trimmed().isEmpty()) {
        return true;
    }

    // Pure numbers
    bool isNumber;
    text.toDouble(&isNumber);
    if (isNumber) {
        return true;
    }

    // Paths (contains /)
    if (text.contains('/')) {
        return true;
    }

    // URLs
    static QRegularExpression urlPattern(
        QStringLiteral("^(https?|ftp|file)://"),
        QRegularExpression::CaseInsensitiveOption
    );
    if (urlPattern.match(text).hasMatch()) {
        return true;
    }

    // File extensions
    static QRegularExpression fileExtPattern(
        QStringLiteral("\\.(png|jpg|jpeg|gif|bmp|wav|ogg|m4a|mp3|json|js)$"),
        QRegularExpression::CaseInsensitiveOption
    );
    if (fileExtPattern.match(text).hasMatch()) {
        return true;
    }

    // RPG Maker system prefixes
    static QStringList systemPrefixes = {
        "img/", "audio/", "data/", "js/", "fonts/",
        "Actor", "Class", "Skill", "Item", "Weapon", "Armor",
        "Enemy", "Troop", "State", "Animation", "Tileset",
        "CommonEvent", "System", "MapInfo"
    };

    for (const QString &prefix : systemPrefixes) {
        if (text.startsWith(prefix, Qt::CaseInsensitive)) {
            return true;
        }
    }

    // RPG Maker control codes (\c[1], \n[1], \v[1], \i[1])
    static QRegularExpression controlCodePattern(
        QStringLiteral("^\\\\[a-z]\\[\\d+\\]$"),
        QRegularExpression::CaseInsensitiveOption
    );
    if (controlCodePattern.match(text.trimmed()).hasMatch()) {
        return true;
    }

    // Generic event names (EV001, EV030, etc.)
    static QRegularExpression eventNamePattern(
        QStringLiteral("^EV\\d{3,}$"),
        QRegularExpression::CaseInsensitiveOption
    );
    if (eventNamePattern.match(text).hasMatch()) {
        return true;
    }

    // Plugin command patterns (technical strings)
    static QRegularExpression pluginCommandPattern(
        QStringLiteral("^[A-Z][a-zA-Z]+ (open|close|add|remove|set|get|show|hide|enable|disable)"),
        QRegularExpression::CaseInsensitiveOption
    );
    if (pluginCommandPattern.match(text).hasMatch()) {
        return true;
    }

    // Variable references ($gameVariables, $gameSwitches, etc.)
    if (text.contains(QStringLiteral("$game"), Qt::CaseInsensitive)) {
        return true;
    }

    // Only symbols/punctuation (no actual text)
    static QRegularExpression symbolOnlyPattern(
        QStringLiteral("^[^a-zA-Z0-9\\p{Thai}\\p{Han}\\p{Hiragana}\\p{Katakana}\\p{Hangul}\\p{Cyrillic}\\p{Arabic}]+$")
    );
    if (symbolOnlyPattern.match(text).hasMatch()) {
        return true;
    }

    // Very short strings (< 2 chars) that are non-translatable
    if (text.length() < 2) {
        static QRegularExpression nonTranslatableShort(
            QStringLiteral("^[a-zA-Z0-9!@#$%^&*()_+=\\-\\[\\]{}|;:'\",.<>?/\\\\]+$")
        );
        if (nonTranslatableShort.match(text).hasMatch()) {
            return true;
        }
    }

    // Single letters or abbreviations (often system values)
    if (text.length() <= 2 && text.toUpper() == text) {
        return true; // HP, MP, ATK, DEF, etc.
    }

    return false;
}

} // namespace rpgm
} // namespace engines
} // namespace core
