#include "PluginManager.h"
#include <QDir>
#include <QDebug>

PluginManager& PluginManager::instance() {
    static PluginManager inst;
    return inst;
}

void PluginManager::loadPlugins(const QString& pluginDir) {
    QDir dir(pluginDir);
    for (const QString& fileName : dir.entryList(QDir::Files)) {
        auto* loader = new QPluginLoader(dir.absoluteFilePath(fileName));
        if (auto* plugin = qobject_cast<IPlugin*>(loader->instance())) {
            if (plugin->initialize()) {
                m_loaders.append(loader);
                m_plugins.append(plugin);
                qDebug() << "Loaded plugin:" << plugin->name();
            }
        } else {
            delete loader;
        }
    }
}

void PluginManager::unloadPlugins() {
    for (auto* plugin : m_plugins) {
        plugin->shutdown();
    }
    qDeleteAll(m_loaders);
    m_loaders.clear();
    m_plugins.clear();
}
