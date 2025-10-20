#ifndef CORE_ENGINES_UNITY_UNITYANALYZER_H
#define CORE_ENGINES_UNITY_UNITYANALYZER_H

#include "core/gameanalyzer.h"

namespace core::engines::unity {

class UnityAnalyzer : public IGameAnalyzer {
public:
    AnalyzerOutput analyze(const QString &inputPath) override;
};

} // namespace core::engines::unity

#endif // CORE_ENGINES_UNITY_UNITYANALYZER_H
