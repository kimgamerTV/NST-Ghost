#include <QCoreApplication>
#include <QSettings>
#include <QDebug>
#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    std::cout << "=== QSettings Debug Tool ===" << std::endl;
    
    // Test with the exact same constructor as we use in the app
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "NST", "PluginSettings");
    
    std::cout << "Settings file location: " << settings.fileName().toStdString() << std::endl;
    std::cout << "Is writable: " << (settings.isWritable() ? "YES" : "NO") << std::endl;
    
    // Write a test value
    settings.setValue("Test/TestKey", "TestValue");
    settings.sync();
    
    std::cout << "Sync status: " << (settings.status() == QSettings::NoError ? "OK" : "ERROR") << std::endl;
    
    // Read it back
    QString readValue = settings.value("Test/TestKey", "NOT_FOUND").toString();
    std::cout << "Read test value: " << readValue.toStdString() << std::endl;
    
    // List all keys
    std::cout << "\nAll settings keys:" << std::endl;
    QStringList keys = settings.allKeys();
    for (const QString &key : keys) {
        std::cout << "  " << key.toStdString() << " = " 
                  << settings.value(key).toString().toStdString() << std::endl;
    }
    
    // Check for groq_translate.lua settings
    std::cout << "\n=== Checking groq_translate.lua settings ===" << std::endl;
    QString apiKey = settings.value("Plugins/groq_translate.lua/Settings/api_key", "NOT_SET").toString();
    QString model = settings.value("Plugins/groq_translate.lua/Settings/model", "NOT_SET").toString();
    bool enabled = settings.value("Plugins/groq_translate.lua/Enabled", false).toBool();
    
    std::cout << "API Key: " << (apiKey == "NOT_SET" ? "NOT_SET" : "***SET***") << std::endl;
    std::cout << "Model: " << model.toStdString() << std::endl;
    std::cout << "Enabled: " << (enabled ? "YES" : "NO") << std::endl;
    
    return 0;
}
