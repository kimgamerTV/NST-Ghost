#include <QtCore/QCoreApplication>
#include <QtCore/QCommandLineParser>
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QTextStream>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("BGA Translator"));

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Consumes analyzer output and produces translation-ready data."));
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption sourceOption(QStringList{QStringLiteral("s"), QStringLiteral("source")},
                                    QStringLiteral("Analyzer JSON output."),
                                    QStringLiteral("path"));
    parser.addOption(sourceOption);

    parser.process(app);

    if (!parser.isSet(sourceOption)) {
        parser.showHelp(EXIT_FAILURE);
    }

    const QString sourcePath = parser.value(sourceOption);
    QFile sourceFile(sourcePath);
    if (!sourceFile.open(QIODevice::ReadOnly)) {
        QTextStream(stderr) << "Failed to open analyzer output: " << sourcePath << '\n';
        return EXIT_FAILURE;
    }

    const QByteArray data = sourceFile.readAll();
    const QJsonDocument document = QJsonDocument::fromJson(data);
    if (!document.isObject()) {
        QTextStream(stderr) << "Invalid analyzer output format." << '\n';
        return EXIT_FAILURE;
    }

    const QJsonObject root = document.object();
    const QJsonArray entries = root.value(QStringLiteral("entries")).toArray();

    QTextStream stdoutStream(stdout);
    for (const QJsonValue &entryValue : entries) {
        const QJsonObject entryObject = entryValue.toObject();
        stdoutStream << entryObject.value(QStringLiteral("id")).toString() << '\t'
                     << entryObject.value(QStringLiteral("text")).toString() << '\n';
    }

    return EXIT_SUCCESS;
}
