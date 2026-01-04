#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QTimer>

#pragma push_macro("slots")
#undef slots
#include <pybind11/embed.h>
#pragma pop_macro("slots")

int main(int argc, char *argv[])
{
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
