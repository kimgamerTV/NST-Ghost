#include "templateanalyzer.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QCommandLineParser>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("BGA Analyzer Template"));

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Generic analyzer template"));
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption inputOption(QStringList{QStringLiteral("i"), QStringLiteral("input")},
                                   QStringLiteral("Input game data path."),
                                   QStringLiteral("path"));
    parser.addOption(inputOption);

    QCommandLineOption outputOption(QStringList{QStringLiteral("o"), QStringLiteral("output")},
                                    QStringLiteral("Output file for extracted data."),
                                    QStringLiteral("path"));
    parser.addOption(outputOption);

    parser.process(app);

    if (!parser.isSet(inputOption) || !parser.isSet(outputOption)) {
        parser.showHelp(EXIT_FAILURE);
    }

    const QString inputPath = parser.value(inputOption);
    const QString outputPath = parser.value(outputOption);

    TemplateAnalyzer analyzer;
    const auto result = analyzer.analyze(inputPath);

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
