#ifndef TEMPLATE_ANALYZER_H
#define TEMPLATE_ANALYZER_H

#include "core/gameanalyzer.h"

class TemplateAnalyzer : public core::IGameAnalyzer {
public:
    core::AnalyzerOutput analyze(const QString &inputPath) override;
};

#endif // TEMPLATE_ANALYZER_H
