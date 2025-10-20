#include <QtTest/QTest>

#include "core/gameanalyzer.h"

class DummyAnalyzer : public core::IGameAnalyzer {
public:
    core::AnalyzerOutput analyze(const QString &inputPath) override
    {
        core::AnalyzerOutput output;
        output.format = QStringLiteral("text/plain");
        output.payload = inputPath.toUtf8();
        return output;
    }
};

class AnalyzerInterfaceTests : public QObject {
    Q_OBJECT

private slots:
    void testAnalyzerOutput()
    {
        DummyAnalyzer analyzer;
        const auto result = analyzer.analyze(QStringLiteral("/tmp/sample"));
        QCOMPARE(result.format, QStringLiteral("text/plain"));
        QCOMPARE(result.payload, QByteArray("/tmp/sample"));
    }
};

QTEST_MAIN(AnalyzerInterfaceTests)

#include "test_analyzer_stub.moc"
