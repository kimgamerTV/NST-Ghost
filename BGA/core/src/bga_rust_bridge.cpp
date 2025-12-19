#include "core/bga_rust_bridge.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QFile>

namespace core {

RustAnalyzerBridge::RustAnalyzerBridge(const QString& engineName)
    : m_engine(engineName.toLower())
{
}

AnalyzerOutput RustAnalyzerBridge::analyze(const QString& inputPath)
{
    AnalyzerOutput output;
    output.format = QStringLiteral("application/json");

    QByteArray engineBytes = m_engine.toUtf8();
    QByteArray pathBytes = inputPath.toUtf8();

    char* result = bga_analyze(engineBytes.constData(), pathBytes.constData());

    if (result == nullptr) {
        output.errorMessage = QStringLiteral("Rust analyzer returned null");
        return output;
    }

    output.payload = QByteArray(result);
    bga_free_string(result);

    return output;
}

bool RustAnalyzerBridge::save(const QString& outputPath, const QJsonArray& texts)
{
    Q_UNUSED(outputPath);

    // Convert QJsonArray to JSON string
    QJsonDocument doc(texts);
    QByteArray jsonBytes = doc.toJson(QJsonDocument::Compact);

    QByteArray engineBytes = m_engine.toUtf8();

    int result = bga_save(engineBytes.constData(), jsonBytes.constData());

    return result == 0;
}

QString RustAnalyzerBridge::getScriptPath(const QString& projectPath) const
{
    if (m_engine != "rpgm") {
        return QString();
    }

    QDir projectDir(projectPath);
    QString scriptPath = projectDir.absoluteFilePath("www/js/rpg_windows.js");
    
    QFile scriptFile(scriptPath);
    if (!scriptFile.exists()) {
        scriptPath = projectDir.absoluteFilePath("js/rpg_windows.js");
    }
    
    return scriptPath;
}

QString RustAnalyzerBridge::getScriptTarget() const
{
    if (m_engine == "rpgm") {
        return QStringLiteral("Window_Base.prototype.convertEscapeCharacters");
    }
    return QString();
}

std::unique_ptr<IGameAnalyzer> createRustAnalyzer(const QString& engineName)
{
    return std::make_unique<RustAnalyzerBridge>(engineName);
}

QStringList availableRustAnalyzers()
{
    char* result = bga_available_analyzers();
    if (result == nullptr) {
        return {};
    }

    QJsonDocument doc = QJsonDocument::fromJson(QByteArray(result));
    bga_free_string(result);

    if (!doc.isArray()) {
        return {};
    }

    QStringList analyzers;
    for (const QJsonValue& val : doc.array()) {
        if (val.isString()) {
            analyzers.append(val.toString());
        }
    }

    return analyzers;
}

} // namespace core
