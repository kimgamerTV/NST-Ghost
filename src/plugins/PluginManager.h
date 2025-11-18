#pragma once
#include <QObject>
#include <QPluginLoader>
#include <QVector>
#include "IPlugin.h"

class PluginManager : public QObject {
    Q_OBJECT
public:
    static PluginManager& instance();
    void loadPlugins(const QString& pluginDir);
    void unloadPlugins();
    QVector<IPlugin*> plugins() const { return m_plugins; }

private:
    PluginManager() = default;
    QVector<QPluginLoader*> m_loaders;
    QVector<IPlugin*> m_plugins;
};
