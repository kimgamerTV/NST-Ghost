#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QTimer>
#include <QDir>
#include <QCoreApplication>
#include <cstdlib>
#include <string>
#include <iostream>

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#pragma push_macro("slots")
#undef slots
#include <pybind11/embed.h>
#pragma pop_macro("slots")

// Helper function to configure Python BEFORE pybind11 initialization
static void configurePythonEnvironment()
{
    // Check if running from AppImage (APPDIR is set by AppRun script)
    const char* appdir = std::getenv("APPDIR");
    
    if (appdir && appdir[0] != '\0') {
        // Running from AppImage - configure bundled Python
        std::string appdir_str(appdir);
        
        // Look for bundled Python in standard locations
        std::string python_home;
        std::string python_lib;
        
        // Try common python installation patterns
        std::vector<std::string> python_versions = {"python3.12", "python3.11", "python3.10", "python3"};
        
        for (const auto& pyver : python_versions) {
            std::string test_path = appdir_str + "/usr/lib/" + pyver;
            std::string test_lib = test_path + "/os.py";  // os.py is always present in stdlib
            
            if (QFile::exists(QString::fromStdString(test_lib))) {
                python_lib = test_path;
                python_home = appdir_str + "/usr";
                break;
            }
        }
        
        // If bundled Python found, configure it
        if (!python_home.empty()) {
            // Convert to wide string for Python API
            std::wstring w_python_home(python_home.begin(), python_home.end());
            
            // Build the module search path
            std::wstring scripts_path = std::wstring(appdir_str.begin(), appdir_str.end()) + L"/usr/bin/scripts";
            std::wstring lib_path(python_lib.begin(), python_lib.end());
            std::wstring site_packages = lib_path + L"/site-packages";
            std::wstring lib_dynload = lib_path + L"/lib-dynload";
            
            // Construct full path: scripts:lib:site-packages:lib-dynload
            std::wstring full_path = scripts_path + L":" + lib_path + L":" + site_packages + L":" + lib_dynload;
            
            // Set Python home and path BEFORE initialization
            Py_SetPythonHome(const_cast<wchar_t*>(w_python_home.c_str()));
            Py_SetPath(const_cast<wchar_t*>(full_path.c_str()));
            
            std::cerr << "[NST] Configured bundled Python home: " << python_home << std::endl;
        } else {
            std::cerr << "[NST] Warning: APPDIR set but no bundled Python found, using system Python" << std::endl;
        }
    } else {
        // Not running from AppImage - use system Python
        // Clear any conflicting environment variables
#ifdef _WIN32
        _putenv("PYTHONHOME=");
#else
        unsetenv("PYTHONHOME");
#endif
        // Let Python auto-discover its paths
        std::cerr << "[NST] Using system Python configuration" << std::endl;
    }
}

int main(int argc, char *argv[])
{
    // Configure Python BEFORE creating interpreter
    configurePythonEnvironment();

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
