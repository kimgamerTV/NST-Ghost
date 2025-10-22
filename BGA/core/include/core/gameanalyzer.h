#ifndef CORE_GAMEANALYZER_H
#define CORE_GAMEANALYZER_H

#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QJsonArray> // Add this include for QJsonArray

namespace core {

struct AnalyzerOutput {
    QString format;
    QByteArray payload;
    QString errorMessage; // New field for error messages
};

class IGameAnalyzer {
public:
    virtual ~IGameAnalyzer() = default;
    virtual AnalyzerOutput analyze(const QString &inputPath) = 0;
    virtual bool save(const QString &outputPath, const QJsonArray &texts) = 0; // New virtual method
};

} // namespace core

#endif // CORE_GAMEANALYZER_H
