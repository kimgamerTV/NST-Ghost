#include <qtlingo/translationservicefactory.h>
#include <qtlingo/translationservice.h>
#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include <iostream>
#include <cassert>
#include <QSettings>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // Enable the plugin programmatically for testing
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "NST", "PluginSettings");
    settings.setValue("Plugins/groq_translate.lua/Enabled", true);
    settings.setValue("Plugins/groq_translate.lua/Installed", true);
    // Also set a dummy API key to ensure it doesn't error out immediately on check
    settings.setValue("Plugins/groq_translate.lua/Settings/api_key", "TEST_KEY");

    qDebug() << "Available services:" << qtlingo::availableTranslationServices();

    auto services = qtlingo::availableTranslationServices();
    QString luaService = "Lua: groq_translate.lua";
    
    if (!services.contains(luaService)) {
        std::cout << "Lua Service '" << luaService.toStdString() << "' not found! (Did enable work?)" << std::endl;
        return 1;
    }

    auto service = qtlingo::createTranslationService(luaService);
    if (!service) {
        std::cout << "Failed to create Lua Service!" << std::endl;
        return 1;
    }

    QObject::connect(service.get(), &qtlingo::ITranslationService::translationFinished, [](const qtlingo::TranslationResult &result) {
        std::cout << "Translation result: " << result.translatedText.toStdString() << std::endl;
        std::cout << "Verification SUCCESS: Script executed with settings." << std::endl;
        QCoreApplication::exit(0);
    });

    QTimer::singleShot(0, [service = service.get()]() {
        service->translate("Hello");
    });

    return app.exec();
}
