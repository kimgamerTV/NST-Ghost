#include "UnrpycPlugin.h"
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

bool UnrpycPlugin::initialize() {
    m_unrpycPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/unrpyc";
    QDir().mkpath(m_unrpycPath);
    qDebug() << "UnrpycPlugin initialized at:" << m_unrpycPath;
    return true;
}

void UnrpycPlugin::shutdown() {
    qDebug() << "UnrpycPlugin shutdown";
}

bool UnrpycPlugin::installUnrpyc() {
    QProcess process;
    process.setWorkingDirectory(m_unrpycPath);
    process.start("git", {"clone", "https://github.com/CensoredUsername/unrpyc.git", "."});
    process.waitForFinished(-1);
    return process.exitCode() == 0;
}

bool UnrpycPlugin::decompile(const QString& rpycFile, const QString& outputDir) {
    QProcess process;
    process.start("python3", {m_unrpycPath + "/unrpyc.py", rpycFile, "-o", outputDir});
    process.waitForFinished(-1);
    return process.exitCode() == 0;
}
