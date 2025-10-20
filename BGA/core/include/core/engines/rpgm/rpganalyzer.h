#ifndef CORE_ENGINES_RPGM_RPGANALYZER_H
#define CORE_ENGINES_RPGM_RPGANALYZER_H

#include "core/gameanalyzer.h"

namespace core::engines::rpgm {

class RpgmAnalyzer : public IGameAnalyzer {
public:
    AnalyzerOutput analyze(const QString &inputPath) override;
};

} // namespace core::engines::rpgm

#endif // CORE_ENGINES_RPGM_RPGANALYZER_H
