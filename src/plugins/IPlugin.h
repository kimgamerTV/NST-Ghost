#pragma once
#include <QString>
#include <QObject>

class QMenu;

class IPlugin {
public:
    virtual ~IPlugin() = default;
    virtual QString name() const = 0;
    virtual QString version() const = 0;
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual bool install() { return true; } // Optional: install dependencies
    virtual void addMenuItems(QMenu* menu) {} // Optional: add menu items
};

Q_DECLARE_INTERFACE(IPlugin, "com.nst.IPlugin/1.0")
