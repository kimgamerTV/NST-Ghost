#pragma once
#include <QObject>
#include "plugins/IPlugin.h"

class MockDataPlugin : public QObject, public IPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.nst.IPlugin/1.0")
    Q_INTERFACES(IPlugin)

public:
    QString name() const override { return "MockDataPlugin"; }
    QString version() const override { return "1.0.0"; }
    bool initialize() override;
    void shutdown() override;

    QStringList getMockTranslations() const;
};
