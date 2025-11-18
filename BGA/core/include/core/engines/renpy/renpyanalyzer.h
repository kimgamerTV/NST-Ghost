#ifndef RENPY_ANALYZER_H
#define RENPY_ANALYZER_H

#include "core/gameanalyzer.h"

namespace core {
namespace engines {
namespace renpy {

class RenpyAnalyzer : public core::IGameAnalyzer {
public:
    core::AnalyzerOutput analyze(const QString &inputPath) override;
    bool save(const QString &outputPath, const QJsonArray &texts) override;
};

} // namespace renpy
} // namespace engines
} // namespace core

#endif // RENPY_ANALYZER_H
