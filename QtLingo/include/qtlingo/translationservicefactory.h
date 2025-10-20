#ifndef QTLINGO_TRANSLATIONSERVICEFACTORY_H
#define QTLINGO_TRANSLATIONSERVICEFACTORY_H

#include "qtlingo/translationservice.h"
#include <memory>
#include <QStringList>

namespace qtlingo {

std::unique_ptr<ITranslationService> createTranslationService(const QString &serviceName, QObject *parent = nullptr);
QStringList availableTranslationServices();

} // namespace qtlingo

#endif // QTLINGO_TRANSLATIONSERVICEFACTORY_H
