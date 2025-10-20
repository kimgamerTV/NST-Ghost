#ifndef RPGM_ANALYZER_H
#define RPGM_ANALYZER_H

#include "core/gameanalyzer.h"

class RpgmAnalyzer : public core::IGameAnalyzer {
public:
    core::AnalyzerOutput analyze(const QString &inputPath) override;
};

#endif // RPGM_ANALYZER_H
