#ifndef UNITY_ANALYZER_H
#define UNITY_ANALYZER_H

#include "core/gameanalyzer.h"

namespace core {
namespace engines {
namespace unity {

class UnityAnalyzer : public core::IGameAnalyzer {
public:
    core::AnalyzerOutput analyze(const QString &inputPath) override;
    bool save(const QString &outputPath, const QJsonArray &texts) override; // Implement save method
};

} // namespace unity
} // namespace engines
} // namespace core

#endif // UNITY_ANALYZER_H
