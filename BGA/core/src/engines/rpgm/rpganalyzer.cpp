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
        QFile file(info.absoluteFilePath());
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QByteArray content = file.readAll();
            file.close();

            QJsonParseError parseError;
            QJsonDocument doc = QJsonDocument::fromJson(content, &parseError);

            if (!doc.isNull()) {
                extractStringsFromJsonValue(
                    QJsonValue::fromVariant(doc.toVariant()),
                    extractedStrings,
                    info.absoluteFilePath(),
                    ""
                    );
                processedCount++;
            } else {
                logStream << "RPGM Analyzer: Failed to parse JSON from file: "
                          << info.absoluteFilePath()
                          << " Error: " << parseError.errorString() << "\n";
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

    // Check if key contains array notation [n]
    if (currentKey.contains('[')) {
        int bracketPos = currentKey.indexOf('[');
        QString baseKey = currentKey.left(bracketPos);
        int arrayIndex = currentKey.mid(bracketPos + 1, currentKey.indexOf(']') - bracketPos - 1).toInt();

        if (!obj.contains(baseKey) || !obj[baseKey].isArray()) return false;

        QJsonArray arr = obj[baseKey].toArray();
        if (index == keys.size() - 1) {
            if (arrayIndex >= 0 && arrayIndex < arr.size()) {
                arr.replace(arrayIndex, newValue);
                obj[baseKey] = arr;
                return true;
            }
        } else {
            if (updateJsonArray(arr, keys, index + 1, newValue)) {
                obj[baseKey] = arr;
                return true;
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

    // Check if it's an array index
    if (currentKey.contains('[')) {
        int bracketPos = currentKey.indexOf('[');
        int arrayIndex = currentKey.mid(bracketPos + 1, currentKey.indexOf(']') - bracketPos - 1).toInt();

        if (arrayIndex < 0 || arrayIndex >= arr.size()) return false;

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
        // กรองข้อความที่ไม่ต้องการแปล
        if (!text.isEmpty() && !isSystemString(text)) {
            QJsonObject entry;
            entry.insert(QStringLiteral("source"), text);
            entry.insert(QStringLiteral("path"), filePath);
            entry.insert(QStringLiteral("key"), currentKeyPath);
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
            QString newKeyPath = currentKeyPath.isEmpty()
            ? QString::number(i)
            : currentKeyPath + "[" + QString::number(i) + "]";
            extractStringsFromJsonValue(arr[i], extractedStrings, filePath, newKeyPath);
        }
    }
}

bool RpgmAnalyzer::isSystemString(const QString &text)
{
    // ตรวจสอบว่ามีแต่ whitespace
    if (text.trimmed().isEmpty()) {
        return true;
    }

    // ตรวจสอบว่าเป็นตัวเลขล้วน
    bool isNumber;
    text.toDouble(&isNumber);
    if (isNumber) {
        return true;
    }

    // ตรวจสอบว่าเป็น path (มี / หรือ \)
    if (text.contains('/') || text.contains('\\')) {
        return true;
    }

    // ตรวจสอบ URL patterns
    static QRegularExpression urlPattern(
        QStringLiteral("^(https?|ftp|file)://"),
        QRegularExpression::CaseInsensitiveOption
        );
    if (urlPattern.match(text).hasMatch()) {
        return true;
    }

    // กรองสตริงที่เป็น system string ไม่ต้องแปล
    static QStringList systemPrefixes = {
        "img/", "audio/", "data/", "js/", "fonts/",
        "Actor", "Class", "Skill", "Item", "Weapon", "Armor",
        "Enemy", "Troop", "State", "Animation", "Tileset",
        "CommonEvent", "System", "MapInfo"
    };

    for (const QString &prefix : systemPrefixes) {
        if (text.startsWith(prefix)) {
            return true;
        }
    }

    // ตรวจสอบ RPG Maker control codes (เช่น \c[1], \n[1], \v[1])
    static QRegularExpression controlCodePattern(
        QStringLiteral("^\\\\[a-z]\\[\\d+\\]$"),
        QRegularExpression::CaseInsensitiveOption
        );
    if (controlCodePattern.match(text.trimmed()).hasMatch()) {
        return true;
    }

    // ตรวจสอบว่ามีแต่ special characters หรือ symbols
    static QRegularExpression symbolOnlyPattern(
        QStringLiteral("^[^a-zA-Z0-9\\u0E00-\\u0E7F\\u4E00-\\u9FFF\\u3040-\\u309F\\u30A0-\\u30FF]+$")
        );
    if (symbolOnlyPattern.match(text).hasMatch()) {
        return true;
    }

    // ตรวจสอบสตริงที่สั้นเกินไป (น้อยกว่า 2 ตัวอักษร) และไม่ใช่ภาษาที่ต้องการแปล
    if (text.length() < 2) {
        static QRegularExpression nonTranslatableShort(
            QStringLiteral("^[a-zA-Z0-9!@#$%^&*()_+=\\-\\[\\]{}|;:'\",.<>?/\\\\]+$")
            );
        if (nonTranslatableShort.match(text).hasMatch()) {
            return true;
        }
    }

    return false;
}

} // namespace rpgm
} // namespace engines
} // namespace core
