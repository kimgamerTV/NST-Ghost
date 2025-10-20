#include "core/analyzerfactory.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QCommandLineParser>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("BGA Runner"));

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Headless analyzer runner"));
    parser.addHelpOption();
    parser.addVersionOption();

    const QStringList engines = core::availableAnalyzers();
    parser.addPositionalArgument(QStringLiteral("engine"),
                                 QStringLiteral("Analyzer engine to run: %1").arg(engines.join(QLatin1String(", "))));
    parser.addPositionalArgument(QStringLiteral("input"), QStringLiteral("Input path"));
    parser.addPositionalArgument(QStringLiteral("output"), QStringLiteral("Output file"));

    parser.process(app);

    const QStringList args = parser.positionalArguments();
    if (args.size() != 3) {
        parser.showHelp(EXIT_FAILURE);
    }

    const QString engine = args.at(0);
    const QString inputPath = args.at(1);
    const QString outputPath = args.at(2);

    auto analyzer = core::createAnalyzer(engine);
    if (!analyzer) {
        QTextStream(stderr) << "Unknown analyzer engine: " << engine << '\n';
        return EXIT_FAILURE;
    }

    const auto result = analyzer->analyze(inputPath);

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
