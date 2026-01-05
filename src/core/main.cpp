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
    // Configure Python paths for AppImage before initializing interpreter
#ifdef __linux__
    const char* appdir = std::getenv("APPDIR");
    if (appdir) {
        // Running from AppImage - use bundled Python at $APPDIR/usr/python
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

    // Load the stylesheet
    QFile file(":/style.qss");
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

     // QTimer::singleShot(0, &w, &MainWindow::onNewProject); // Optional: Auto-start new project flow // Optional: Auto-start new project flow


    return a.exec();
}
