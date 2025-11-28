#ifndef QTLINGO_TRANSLATIONPLUGININTERFACE_H
#define QTLINGO_TRANSLATIONPLUGININTERFACE_H

#include "QtLingo_global.h"
#include "translationservice.h"
#include <QStringList>
#include <QtPlugin>

namespace qtlingo {

class TranslationPluginInterface
{
public:
    virtual ~TranslationPluginInterface() = default;

    // Returns a list of service names (keys) provided by this plugin.
    virtual QStringList keys() const = 0;

    // Creates a new instance of the translation service for the given key.
    virtual ITranslationService* create(const QString &key, QObject *parent = nullptr) = 0;
};

} // namespace qtlingo

#define TranslationPluginInterface_iid "org.qtlingo.TranslationPluginInterface"

Q_DECLARE_INTERFACE(qtlingo::TranslationPluginInterface, TranslationPluginInterface_iid)

#endif // QTLINGO_TRANSLATIONPLUGININTERFACE_H
