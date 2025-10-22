#ifndef RPGM_ANALYZER_H
#define RPGM_ANALYZER_H

#include "core/gameanalyzer.h"

namespace core {
namespace engines {
namespace rpgm {

class RpgmAnalyzer : public core::IGameAnalyzer {
public:
    core::AnalyzerOutput analyze(const QString &inputPath) override;
    bool save(const QString &outputPath, const QJsonArray &texts) override; // Implement save method
private:
    void extractStringsFromJsonValue(const QJsonValue &jsonValue, QJsonArray &extractedStrings, const QString &filePath, const QString &currentKeyPath = ""); // New helper function
};

} // namespace rpgm
} // namespace engines
} // namespace core

#endif // RPGM_ANALYZER_H
