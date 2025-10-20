#include "unityanalyzer.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QCommandLineParser>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("BGA Unity Analyzer"));

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Extracts text assets from Unity projects."));
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption projectOption(QStringList{QStringLiteral("p"), QStringLiteral("project")},
                                     QStringLiteral("Path to Unity project directory."),
                                     QStringLiteral("path"));
    parser.addOption(projectOption);

    QCommandLineOption outputOption(QStringList{QStringLiteral("o"), QStringLiteral("output")},
                                    QStringLiteral("Output file for extracted data."),
                                    QStringLiteral("path"));
    parser.addOption(outputOption);

    parser.process(app);

    if (!parser.isSet(projectOption) || !parser.isSet(outputOption)) {
        parser.showHelp(EXIT_FAILURE);
    }

    const QString projectPath = parser.value(projectOption);
    const QString outputPath = parser.value(outputOption);

    UnityAnalyzer analyzer;
    const auto result = analyzer.analyze(projectPath);

    QFile outputFile(outputPath);
    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QTextStream(stderr) << "Failed to open output file: " << outputPath << '\n';
        return EXIT_FAILURE;
    }

    if (outputFile.write(result.payload) == -1) {
        QTextStream(stderr) << "Failed to write analyzer output" << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
