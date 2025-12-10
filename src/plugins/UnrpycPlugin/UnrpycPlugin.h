#pragma once
#include <QObject>
#include <QProcess>
#include "plugins/IPlugin.h"

class UnrpycPlugin : public QObject, public IPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.nst.IPlugin/1.0")
    Q_INTERFACES(IPlugin)

public:
    QString name() const override { return "UnrpycPlugin"; }
    QString version() const override { return "1.0.0"; }
    bool initialize() override;
    void shutdown() override;

    bool installUnrpyc();
    bool decompile(const QString& rpycFile, const QString& outputDir);

private:
    QString m_unrpycPath;
};
