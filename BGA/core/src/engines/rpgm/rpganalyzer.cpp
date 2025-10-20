#include "core/engines/rpgm/rpganalyzer.h"

#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QFile>
#include <QtCore/QFileInfoList>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>

namespace {

QString joinKey(const QString &base, const QString &child)
{
    if (base.isEmpty()) {
        return child;
    }
    return base + QLatin1Char('/') + child;
}

void collectTexts(const QJsonValue &value, const QString &keyPath, const QString &filePath, QJsonArray &entries)
{
    if (value.isObject()) {
        const QJsonObject object = value.toObject();
        const auto keys = object.keys();
        for (const QString &key : keys) {
            collectTexts(object.value(key), joinKey(keyPath, key), filePath, entries);
        }
        return;
    }

    if (value.isArray()) {
        const QJsonArray array = value.toArray();
        for (int i = 0; i < array.size(); ++i) {
            collectTexts(array.at(i), joinKey(keyPath, QString::number(i)), filePath, entries);
        }
        return;
    }

    if (value.isString()) {
        QJsonObject entry;
        entry.insert(QStringLiteral("file"), filePath);
        if (!keyPath.isEmpty()) {
            entry.insert(QStringLiteral("key"), keyPath);
        }
        entry.insert(QStringLiteral("text"), value.toString());
        entries.append(entry);
    }
}

bool isImageSuffix(const QString &suffix)
{
    static const QStringList imageSuffixes = {
        QStringLiteral("png"), QStringLiteral("jpg"), QStringLiteral("jpeg"), QStringLiteral("bmp"),
        QStringLiteral("gif"), QStringLiteral("webp"), QStringLiteral("tga")
    };
    return imageSuffixes.contains(suffix, Qt::CaseInsensitive);
}

bool isFontSuffix(const QString &suffix)
{
    static const QStringList fontSuffixes = {
        QStringLiteral("ttf"), QStringLiteral("otf"), QStringLiteral("ttc"), QStringLiteral("fnt")
    };
    return fontSuffixes.contains(suffix, Qt::CaseInsensitive);
}

} // namespace

namespace core::engines::rpgm {

AnalyzerOutput RpgmAnalyzer::analyze(const QString &inputPath)
{
    QDir projectDir(inputPath);
    const QFileInfoList jsonFiles = projectDir.entryInfoList(QStringList{QStringLiteral("*.json")}, QDir::Files);

    QJsonArray textEntries;
    for (const QFileInfo &info : jsonFiles) {
        QFile file(info.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly)) {
            continue;
        }
        const QByteArray data = file.readAll();
        file.close();

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isNull()) {
            continue;
        }
        collectTexts(doc.isArray() ? QJsonValue(doc.array()) : QJsonValue(doc.object()), QString(), info.absoluteFilePath(), textEntries);
    }

    QJsonArray imageEntries;
    QDirIterator imageIt(inputPath, QDir::Files, QDirIterator::Subdirectories);
    while (imageIt.hasNext()) {
        imageIt.next();
        const QFileInfo info = imageIt.fileInfo();
        if (!isImageSuffix(info.suffix())) {
            continue;
        }
        QJsonObject entry;
        entry.insert(QStringLiteral("path"), info.absoluteFilePath());
        entry.insert(QStringLiteral("name"), info.fileName());
        imageEntries.append(entry);
    }

    QJsonArray fontEntries;
    QDirIterator fontIt(inputPath, QDir::Files, QDirIterator::Subdirectories);
    while (fontIt.hasNext()) {
        fontIt.next();
        const QFileInfo info = fontIt.fileInfo();
        if (!isFontSuffix(info.suffix())) {
            continue;
        }
        QJsonObject entry;
        entry.insert(QStringLiteral("path"), info.absoluteFilePath());
        entry.insert(QStringLiteral("name"), info.completeBaseName());
        fontEntries.append(entry);
    }

    QJsonObject root;
    root.insert(QStringLiteral("engine"), QStringLiteral("rpgm"));
    root.insert(QStringLiteral("source"), inputPath);
    root.insert(QStringLiteral("texts"), textEntries);
    root.insert(QStringLiteral("images"), imageEntries);
    root.insert(QStringLiteral("fonts"), fontEntries);

    QJsonDocument document(root);

    AnalyzerOutput output;
    output.format = QStringLiteral("application/json");
    output.payload = document.toJson(QJsonDocument::Indented);
    return output;
}

} // namespace core::engines::rpgm
