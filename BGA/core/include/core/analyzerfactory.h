#ifndef CORE_ANALYZERFACTORY_H
#define CORE_ANALYZERFACTORY_H

#include "core/gameanalyzer.h"

#include <memory>

#include <QtCore/QString>
#include <QtCore/QStringList>

namespace core {

std::unique_ptr<IGameAnalyzer> createAnalyzer(const QString &engineName);
QStringList availableAnalyzers();

} // namespace core

#endif // CORE_ANALYZERFACTORY_H
