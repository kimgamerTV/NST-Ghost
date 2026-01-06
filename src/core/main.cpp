#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QTimer>
#include <cstdlib>
#include <string>

#pragma push_macro("slots")
#undef slots
#include <pybind11/embed.h>
#pragma pop_macro("slots")

int main(int argc, char *argv[])
{
    // Fix: Restore Py_SetPythonHome for bundled Python detection (AppImage/tar.gz)
    // This is required when running from the bundled environment to find stdlib
#ifdef __linux__
    const char* appdir = std::getenv("APPDIR");
    if (appdir) {
        static std::wstring pythonHome;
        std::string appdirStr(appdir);
        pythonHome = std::wstring(appdirStr.begin(), appdirStr.end()) + L"/usr/python";
        Py_SetPythonHome(pythonHome.c_str());
    }
#endif

    // Initialize Python Interpreter
    pybind11::scoped_interpreter guard{};
    pybind11::gil_scoped_release release;

    QApplication a(argc, argv);

    // Fix: Correct resource path for stylesheet (was :/style.qss, needed :/ui/style.qss)
    QFile file(":/ui/style.qss");
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&file);
        a.setStyleSheet(stream.readAll());
        file.close();
    }

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "NST_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.show();

    // QTimer::singleShot(0, &w, &MainWindow::onNewProject); // Optional: Auto-start new project flow

    return a.exec();
}
