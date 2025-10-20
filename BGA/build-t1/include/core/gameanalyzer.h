#ifndef CORE_GAMEANALYZER_H
#define CORE_GAMEANALYZER_H

#include <QtCore/QByteArray>
#include <QtCore/QString>

namespace core {

struct AnalyzerOutput {
    QString format;
    QByteArray payload;
};

class IGameAnalyzer {
public:
    virtual ~IGameAnalyzer() = default;
    virtual AnalyzerOutput analyze(const QString &inputPath) = 0;
};

} // namespace core

#endif // CORE_GAMEANALYZER_H
