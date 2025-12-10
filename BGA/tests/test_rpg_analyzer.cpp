#include <QtTest/QtTest>
#include "core/engines/rpgm/rpganalyzer.h"
#include <QDir>
#include <QFile>
#include <QDirIterator>
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
        
        QDir dir(tempDir->path());
        dir.mkdir("data");
        dataPath = dir.filePath("data");

        // Copy ALL json files from Unit-Test/RPGM recursively to dataPath
        QString sourceRoot = "/home/jop/work/NST/NST/Unit-Test/RPGM";
        QDirIterator it(sourceRoot, QStringList() << "*.json", QDir::Files, QDirIterator::Subdirectories);
        
        while (it.hasNext()) {
            QString sourcePath = it.next();
            QString fileName = it.fileName();
            // Flatten the structure for simplicity in data folder, or just copy to root of data
            // RPGM usually expects them in specific places, but analyzer might just scan all.
            // Let's copy to data/fileName to simulate standard data folder content.
            // If filenames collide (e.g. Map001.json in dialogs and dialogs_gaya), we might issue.
            // Let's check if we need to rename or keep structure.
            // The analyzer searches recursively in project root, or in "data" folder.
            // It searches "data"/*.json.
            
            // For this coverage test, we want to capture EVERYTHING.
            // Let's copy them with unique names if needed, or just copy into data/.
            
            QString destPath = dataPath + "/" + fileName;
            if (QFile::exists(destPath)) {
                destPath = dataPath + "/" + it.fileInfo().baseName() + "_" + QString::number(QRandomGenerator::global()->generate()) + ".json";
            }
            QFile::copy(sourcePath, destPath);
        }
    }

    void testCoverage()
    {
        core::engines::rpgm::RpgmAnalyzer analyzer;
        core::AnalyzerOutput output = analyzer.analyze(tempDir->path()); 

        QJsonDocument doc = QJsonDocument::fromJson(output.payload);
        QJsonArray strings = doc.object()["strings"].toArray();
        
        qDebug() << "==========================================";
        qDebug() << "          EXTRACTION COVERAGE REPORT      ";
        qDebug() << "==========================================";
        qDebug() << "Total Strings Extracted: " << strings.size();
        
        QMap<QString, int> fileCounts;

        for(const auto& val : strings) {
            QJsonObject obj = val.toObject();
            QString path = obj["path"].toString();
            QString fileName = QFileInfo(path).fileName();
            fileCounts[fileName]++;
            
            // Print details for manual verification if needed
            // qDebug() << "[" << fileName << "] " << obj["key"].toString() << " : " << obj["source"].toString();
        }

        for(auto it = fileCounts.begin(); it != fileCounts.end(); ++it) {
            qDebug() << "File:" << it.key() << " - Extracted:" << it.value();
        }
        
        // Also print out the actual strings to stdout for capture
        for(const auto& val : strings) {
            QJsonObject obj = val.toObject();
             // Clean newlines for cleaner output
            QString source = obj["source"].toString().replace("\n", "\\n");
            qInfo() << "[EXTRACTED] " << QFileInfo(obj["path"].toString()).fileName() << " | " << obj["key"].toString() << " | " << source;
        }

        qDebug() << "==========================================";
        
        QVERIFY(strings.size() > 0);
    }
};

QTEST_MAIN(TestRpgAnalyzer)
#include "test_rpg_analyzer.moc"
