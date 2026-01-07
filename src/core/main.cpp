// Python includes - only when HAS_PYTHON is defined
#ifdef HAS_PYTHON
// IMPORTANT: Python.h must be included FIRST, before any Qt headers
// to avoid Qt's "slots" macro conflict with Python's use of "slots"
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#pragma push_macro("slots")
#undef slots
#include <pybind11/embed.h>
#pragma pop_macro("slots")
#endif

// Now we can include Qt headers
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
#include <vector>



#ifdef HAS_PYTHON
// Helper function to configure Python BEFORE pybind11 initialization
// Uses Python 3.11+ PyConfig API to avoid deprecated functions
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
        
        // If bundled Python found, configure it using environment variables
        // This is more reliable than deprecated Py_SetPythonHome/Py_SetPath
        if (!python_home.empty()) {
            // Set PYTHONHOME environment variable (works with all Python versions)
            std::string pythonhome_env = "PYTHONHOME=" + python_home;
            putenv(const_cast<char*>(strdup(pythonhome_env.c_str())));
            
            // Build PYTHONPATH
            std::string scripts_path = appdir_str + "/usr/bin";  // Parent dir so "import scripts" works
            std::string site_packages = python_lib + "/site-packages";
            std::string lib_dynload = python_lib + "/lib-dynload";
            
            std::string pythonpath = scripts_path + ":" + python_lib + ":" + site_packages + ":" + lib_dynload;
            std::string pythonpath_env = "PYTHONPATH=" + pythonpath;
            putenv(const_cast<char*>(strdup(pythonpath_env.c_str())));
            
            std::cerr << "[NST] Configured bundled Python home: " << python_home << std::endl;
            std::cerr << "[NST] PYTHONPATH: " << pythonpath << std::endl;
        } else {
            std::cerr << "[NST] Warning: APPDIR set but no bundled Python found, using system Python" << std::endl;
            // Clear potentially conflicting environment
#ifdef _WIN32
            _putenv("PYTHONHOME=");
#else
            unsetenv("PYTHONHOME");
#endif
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
#endif // HAS_PYTHON

int main(int argc, char *argv[])
{
#ifdef HAS_PYTHON
    // Configure Python BEFORE creating interpreter
    configurePythonEnvironment();

    // Initialize Python Interpreter
    pybind11::scoped_interpreter guard{};
    pybind11::gil_scoped_release release;
#endif

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
