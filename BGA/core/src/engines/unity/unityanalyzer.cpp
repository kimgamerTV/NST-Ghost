#include "core/engines/unity/unityanalyzer.h"

#include <QtCore/QDirIterator>
#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>

namespace {

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
        QStringLiteral("ttf"), QStringLiteral("otf"), QStringLiteral("ttc"), QStringLiteral("fnt"),
        QStringLiteral("fontsettings"), QStringLiteral("dfont")
    };
    return fontSuffixes.contains(suffix, Qt::CaseInsensitive);
}

void collectUnityStrings(const QJsonValue &value, const QString &keyPath, const QString &filePath, QJsonArray &entries)
{
    if (value.isObject()) {
        const QJsonObject object = value.toObject();
        for (auto it = object.constBegin(); it != object.constEnd(); ++it) {
            const QString nextPath = keyPath.isEmpty() ? it.key() : keyPath + QLatin1Char('/') + it.key();
            collectUnityStrings(it.value(), nextPath, filePath, entries);
        }
        return;
    }

    if (value.isArray()) {
        const QJsonArray array = value.toArray();
        for (int i = 0; i < array.size(); ++i) {
            const QString nextPath = keyPath.isEmpty() ? QString::number(i) : keyPath + QLatin1Char('/') + QString::number(i);
            collectUnityStrings(array.at(i), nextPath, filePath, entries);
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

} // namespace

namespace core::engines::unity {

AnalyzerOutput UnityAnalyzer::analyze(const QString &inputPath)
{
    QJsonArray textEntries;
    QDirIterator textIterator(inputPath, QStringList{QStringLiteral("*.json"), QStringLiteral("*.asset"), QStringLiteral("*.prefab")},
                               QDir::Files, QDirIterator::Subdirectories);
    while (textIterator.hasNext()) {
        const QString path = textIterator.next();
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) {
            continue;
        }
        const QByteArray data = file.readAll();
        file.close();

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isNull()) {
            continue;
        }

        if (doc.isArray()) {
            collectUnityStrings(QJsonValue(doc.array()), QString(), path, textEntries);
        } else {
            collectUnityStrings(QJsonValue(doc.object()), QString(), path, textEntries);
        }
    }

    QJsonArray imageEntries;
    QDirIterator imageIterator(inputPath, QDir::Files, QDirIterator::Subdirectories);
    while (imageIterator.hasNext()) {
        imageIterator.next();
        const QFileInfo info = imageIterator.fileInfo();
        if (!isImageSuffix(info.suffix())) {
            continue;
        }
        QJsonObject entry;
        entry.insert(QStringLiteral("path"), info.absoluteFilePath());
        entry.insert(QStringLiteral("name"), info.fileName());
        imageEntries.append(entry);
    }

    QJsonArray fontEntries;
    QDirIterator fontIterator(inputPath, QDir::Files, QDirIterator::Subdirectories);
    while (fontIterator.hasNext()) {
        fontIterator.next();
        const QFileInfo info = fontIterator.fileInfo();
        if (!isFontSuffix(info.suffix())) {
            continue;
        }
        QJsonObject entry;
        entry.insert(QStringLiteral("path"), info.absoluteFilePath());
        entry.insert(QStringLiteral("name"), info.completeBaseName());
        fontEntries.append(entry);
    }

    QJsonObject root;
    root.insert(QStringLiteral("engine"), QStringLiteral("unity"));
    root.insert(QStringLiteral("source"), inputPath);
    root.insert(QStringLiteral("texts"), textEntries);
    root.insert(QStringLiteral("images"), imageEntries);
    root.insert(QStringLiteral("fonts"), fontEntries);

    QJsonDocument doc(root);

    AnalyzerOutput output;
    output.format = QStringLiteral("application/json");
    output.payload = doc.toJson(QJsonDocument::Indented);
    return output;
}

} // namespace core::engines::unity
