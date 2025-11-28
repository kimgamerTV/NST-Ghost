#ifndef MOCKPLUGIN_H
#define MOCKPLUGIN_H

#include <QObject>
#include <qtlingo/translationplugininterface.h>

class MockTranslationService : public qtlingo::ITranslationService
{
    Q_OBJECT
public:
    explicit MockTranslationService(QObject *parent = nullptr) : qtlingo::ITranslationService(parent) {}
    QString serviceName() const override { return "Mock Service"; }
    void translate(const QString &sourceText) override {
        emit translationFinished({sourceText, "[Mock] " + sourceText});
    }
};

class MockTranslationPlugin : public QObject, public qtlingo::TranslationPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qtlingo.TranslationPluginInterface")
    Q_INTERFACES(qtlingo::TranslationPluginInterface)

public:
    QStringList keys() const override { return {"Mock Service"}; }
    qtlingo::ITranslationService* create(const QString &key, QObject *parent = nullptr) override {
        if (key == "Mock Service") {
            return new MockTranslationService(parent);
        }
        return nullptr;
    }
};

#endif // MOCKPLUGIN_H
