#ifndef QTLINGO_TRANSLATIONSERVICEFACTORY_H
#define QTLINGO_TRANSLATIONSERVICEFACTORY_H

#include "QtLingo_global.h"
#include "qtlingo/translationservice.h"
#include <memory>
#include <QStringList>

namespace qtlingo {

QTLINGO_EXPORT std::unique_ptr<ITranslationService> createTranslationService(const QString &serviceName, QObject *parent = nullptr);
QTLINGO_EXPORT QStringList availableTranslationServices();

} // namespace qtlingo

#endif // QTLINGO_TRANSLATIONSERVICEFACTORY_H
