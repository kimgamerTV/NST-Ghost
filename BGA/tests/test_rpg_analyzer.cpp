#include <QtTest/QtTest>
#include "core/engines/rpgm/rpganalyzer.h"
#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

class TestRpgAnalyzer : public QObject
{
    Q_OBJECT

private:
    std::unique_ptr<QTemporaryDir> tempDir;
    QString dataPath;

private slots:
    void initTestCase()
    {
        tempDir = std::make_unique<QTemporaryDir>();
        QVERIFY(tempDir->isValid());
        
        // Setup mock project structure
        QDir dir(tempDir->path());
        dir.mkdir("data");
        dataPath = dir.filePath("data");

        // Copy test files
        QString sourceRoot = "/home/jop/work/NST/NST/Unit-Test/RPGM";
        
        // Check if source exists
        QVERIFY2(QDir(sourceRoot).exists(), "Source mock data directory missing!");

        QVERIFY(QFile::copy(sourceRoot + "/dialogs/Map001.json", dataPath + "/Map001.json"));
        QVERIFY(QFile::copy(sourceRoot + "/objects/Actors.json", dataPath + "/Actors.json"));
    }

    void testAnalyze()
    {
        core::engines::rpgm::RpgmAnalyzer analyzer;
        core::AnalyzerOutput output = analyzer.analyze(tempDir->path()); // Analyze root, should find data/

        QCOMPARE(output.format, QString("application/json"));
        
        QJsonDocument doc = QJsonDocument::fromJson(output.payload);
        QVERIFY(!doc.isNull());
        QVERIFY(doc.isObject());
        
        QJsonObject root = doc.object();
        QVERIFY(root.contains("strings"));
        QJsonArray strings = root["strings"].toArray();
        QVERIFY(strings.size() > 0);

        // Check for specific known strings from Map001.json
        // {"code":401,"indent":0,"parameters":["All'orizzonte il cielo blu andava a cadere nelle"]}
        bool foundMapText = false;
        for(const auto& val : strings) {
            QJsonObject obj = val.toObject();
            if (obj["source"].toString().contains("All'orizzonte il cielo blu")) {
                foundMapText = true;
                QCOMPARE(obj["key"].toString(), QString("events[0].pages[0].list[4].parameters[0]")); 
                // Note: The key path depends on array indices, events array index 0.
                // Map001.json: "events": [null, {"id":1,...}] usually in RPGM events array has null at 0.
                // But the snippet showed: "events":[ {"id":1...} ] so index 0 is valid.
                break;
            }
        }
        QVERIFY(foundMapText);
        
        // Check for Actors.json extraction (whitelisted keys)
        // Actors.json: "name": "Destiny"
        bool foundActorName = false;
        bool foundActorProfile = false;
        bool foundNonWhitelistedKey = false;

        for(const auto& val : strings) {
            QJsonObject obj = val.toObject();
            if (obj["path"].toString().endsWith("Actors.json")) {
                if (obj["source"].toString() == "Destiny") {
                    foundActorName = true;
                }
                if (obj["source"].toString().contains("Un ragazzo misterioso")) {
                    foundActorProfile = true;
                }
                // Check for keys that should NOT be here (e.g. battlerName)
                if (obj["key"].toString().contains("battlerName")) {
                    foundNonWhitelistedKey = true;
                }
            }
        }
        QVERIFY(foundActorName);
        QVERIFY(foundActorProfile);
        QVERIFY(!foundNonWhitelistedKey); // Ensure non-whitelisted keys are ignored
    }
};

QTEST_MAIN(TestRpgAnalyzer)
#include "test_rpg_analyzer.moc"
