/********************************************************************************
** Form generated from reading UI file 'filetranslationwidget.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FILETRANSLATIONWIDGET_H
#define UI_FILETRANSLATIONWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListView>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FileTranslationWidget
{
public:
    QVBoxLayout *verticalLayout;
    QSplitter *splitter;
    QListView *fileListView;
    QTableView *translationTableView;

    void setupUi(QWidget *FileTranslationWidget)
    {
        if (FileTranslationWidget->objectName().isEmpty())
            FileTranslationWidget->setObjectName("FileTranslationWidget");
        FileTranslationWidget->resize(800, 600);
        verticalLayout = new QVBoxLayout(FileTranslationWidget);
        verticalLayout->setObjectName("verticalLayout");
        splitter = new QSplitter(FileTranslationWidget);
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

        verticalLayout->addWidget(splitter);


        retranslateUi(FileTranslationWidget);

        QMetaObject::connectSlotsByName(FileTranslationWidget);
    } // setupUi

    void retranslateUi(QWidget *FileTranslationWidget)
    {
        FileTranslationWidget->setWindowTitle(QCoreApplication::translate("FileTranslationWidget", "File Translation", nullptr));
    } // retranslateUi

};

namespace Ui {
    class FileTranslationWidget: public Ui_FileTranslationWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FILETRANSLATIONWIDGET_H
