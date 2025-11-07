/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionLoad_from_Game_Project;
    QAction *actionSettings;
    QAction *action_Exit;
    QAction *actionOpen_Mock_Data;
    QAction *actionTranslate_Selected;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QSplitter *splitter;
    QListView *fileListView;
    QTableView *translationTableView;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuTranslate;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        actionLoad_from_Game_Project = new QAction(MainWindow);
        actionLoad_from_Game_Project->setObjectName("actionLoad_from_Game_Project");
        actionSettings = new QAction(MainWindow);
        actionSettings->setObjectName("actionSettings");
        action_Exit = new QAction(MainWindow);
        action_Exit->setObjectName("action_Exit");
        actionOpen_Mock_Data = new QAction(MainWindow);
        actionOpen_Mock_Data->setObjectName("actionOpen_Mock_Data");
        actionTranslate_Selected = new QAction(MainWindow);
        actionTranslate_Selected->setObjectName("actionTranslate_Selected");
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName("horizontalLayout");
        splitter = new QSplitter(centralwidget);
        splitter->setObjectName("splitter");
        splitter->setOrientation(Qt::Horizontal);
        fileListView = new QListView(splitter);
        fileListView->setObjectName("fileListView");
        splitter->addWidget(fileListView);
        translationTableView = new QTableView(splitter);
        translationTableView->setObjectName("translationTableView");
        translationTableView->setAlternatingRowColors(true);
        translationTableView->setSortingEnabled(true);
        splitter->addWidget(translationTableView);

        horizontalLayout->addWidget(splitter);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 23));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName("menuFile");
        menuTranslate = new QMenu(menubar);
        menuTranslate->setObjectName("menuTranslate");
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuTranslate->menuAction());
        menuFile->addAction(actionOpen_Mock_Data);
        menuFile->addAction(actionLoad_from_Game_Project);
        menuFile->addAction(actionSettings);
        menuFile->addSeparator();
        menuFile->addAction(action_Exit);
        menuTranslate->addAction(actionTranslate_Selected);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "NST Ghost", nullptr));
        actionLoad_from_Game_Project->setText(QCoreApplication::translate("MainWindow", "Load from Game Project...", nullptr));
        actionSettings->setText(QCoreApplication::translate("MainWindow", "Settings...", nullptr));
        action_Exit->setText(QCoreApplication::translate("MainWindow", "E&xit", nullptr));
        actionOpen_Mock_Data->setText(QCoreApplication::translate("MainWindow", "Open Mock Data", nullptr));
        actionTranslate_Selected->setText(QCoreApplication::translate("MainWindow", "Translate Selected", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "&File", nullptr));
        menuTranslate->setTitle(QCoreApplication::translate("MainWindow", "&Translate", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
