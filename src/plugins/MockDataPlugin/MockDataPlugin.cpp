#include "MockDataPlugin.h"
#include <QDebug>

bool MockDataPlugin::initialize() {
    qDebug() << "MockDataPlugin initialized";
    return true;
}

void MockDataPlugin::shutdown() {
    qDebug() << "MockDataPlugin shutdown";
}

QStringList MockDataPlugin::getMockTranslations() const {
    return {
        "Hello World|สวัสดีชาวโลก",
        "Start Game|เริ่มเกม",
        "Load Game|โหลดเกม",
        "Settings|ตั้งค่า",
        "Exit|ออก"
    };
}
