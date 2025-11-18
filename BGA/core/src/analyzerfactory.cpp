#include "core/analyzerfactory.h"

#include "core/engines/rpgm/rpganalyzer.h"
#include "core/engines/unity/unityanalyzer.h"
#include "core/engines/renpy/renpyanalyzer.h"


namespace core {

std::unique_ptr<IGameAnalyzer> createAnalyzer(const QString &engineName)
{
    if (engineName.compare(QStringLiteral("rpgm"), Qt::CaseInsensitive) == 0) {
        return std::make_unique<engines::rpgm::RpgmAnalyzer>();
    }
    if (engineName.compare(QStringLiteral("unity"), Qt::CaseInsensitive) == 0) {
        return std::make_unique<engines::unity::UnityAnalyzer>();
    }
    if (engineName.compare(QStringLiteral("renpy"), Qt::CaseInsensitive) == 0) {
        return std::make_unique<engines::renpy::RenpyAnalyzer>();
    }
    return nullptr;
}

QStringList availableAnalyzers()
{
    return {QStringLiteral("rpgm"), QStringLiteral("unity"), QStringLiteral("renpy")};
}

} // namespace core
