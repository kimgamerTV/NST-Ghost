#ifndef UNITY_ANALYZER_H
#define UNITY_ANALYZER_H

#include "core/gameanalyzer.h"

class UnityAnalyzer : public core::IGameAnalyzer {
public:
    core::AnalyzerOutput analyze(const QString &inputPath) override;
};

#endif // UNITY_ANALYZER_H
