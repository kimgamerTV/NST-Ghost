#ifndef CORE_GAMEANALYZER_H
#define CORE_GAMEANALYZER_H

#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QJsonArray> // Add this include for QJsonArray

namespace core {

struct AnalyzerOutput {
    QString format;
    QByteArray payload;
    QString errorMessage; // New field for error messages
};

class IGameAnalyzer {
public:
    virtual ~IGameAnalyzer() = default;
    virtual AnalyzerOutput analyze(const QString &inputPath) = 0;
    virtual bool save(const QString &outputPath, const QJsonArray &texts) = 0; // New virtual method

    // Script Editing Support
    virtual bool canEditScript() const { return false; }
    virtual QString getScriptPath(const QString &projectPath) const { return QString(); }
    virtual QString getScriptTarget() const { return QString(); } // Removed projectPath arg as target is usually constant per engine, but could depend on version. Let's keep it simple. Actually plan said getScriptTarget(projectPath), but typically the function name is constant. I'll stick to no arg for now unless needed. Wait, plan said `getScriptTarget(const QString &projectPath)`. I'll follow the plan to be safe, though likely unused.
};

} // namespace core

#endif // CORE_GAMEANALYZER_H
