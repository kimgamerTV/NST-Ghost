#ifndef RPGM_ANALYZER_H
#define RPGM_ANALYZER_H

#include "core/gameanalyzer.h"

namespace core {
namespace engines {
namespace rpgm {

class RpgmAnalyzer : public core::IGameAnalyzer {
public:
    core::    AnalyzerOutput analyze(const QString &inputPath) override;
    bool save(const QString &outputPath, const QJsonArray &texts) override;

    bool canEditScript() const override { return true; }
    QString getScriptPath(const QString &projectPath) const override;
    QString getScriptTarget() const override; // Matching base class signature
 // Implement save method
private:
    void extractStringsFromJsonValue(const QJsonValue &jsonValue, QJsonArray &extractedStrings, const QString &filePath, const QString &currentKeyPath = "");
    bool updateJsonValue(QJsonDocument &doc, const QString &keyPath, const QString &newValue);
    bool updateJsonObject(QJsonObject &obj, const QStringList &keys, int index, const QString &newValue);
    bool updateJsonArray(QJsonArray &arr, const QStringList &keys, int index, const QString &newValue);
    bool isSystemString(const QString &text);
};

} // namespace rpgm
} // namespace engines
} // namespace core

#endif // RPGM_ANALYZER_H
