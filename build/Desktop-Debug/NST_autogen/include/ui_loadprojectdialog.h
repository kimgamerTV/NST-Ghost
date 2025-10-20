/********************************************************************************
** Form generated from reading UI file 'loadprojectdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOADPROJECTDIALOG_H
#define UI_LOADPROJECTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_LoadProjectDialog
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *engineLayout;
    QLabel *engineLabel;
    QComboBox *engineComboBox;
    QHBoxLayout *pathLayout;
    QLabel *pathLabel;
    QLineEdit *projectPathLineEdit;
    QPushButton *browseButton;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *LoadProjectDialog)
    {
        if (LoadProjectDialog->objectName().isEmpty())
            LoadProjectDialog->setObjectName("LoadProjectDialog");
        LoadProjectDialog->resize(400, 150);
        verticalLayout = new QVBoxLayout(LoadProjectDialog);
        verticalLayout->setObjectName("verticalLayout");
        engineLayout = new QHBoxLayout();
        engineLayout->setObjectName("engineLayout");
        engineLabel = new QLabel(LoadProjectDialog);
        engineLabel->setObjectName("engineLabel");

        engineLayout->addWidget(engineLabel);

        engineComboBox = new QComboBox(LoadProjectDialog);
        engineComboBox->setObjectName("engineComboBox");

        engineLayout->addWidget(engineComboBox);


        verticalLayout->addLayout(engineLayout);

        pathLayout = new QHBoxLayout();
        pathLayout->setObjectName("pathLayout");
        pathLabel = new QLabel(LoadProjectDialog);
        pathLabel->setObjectName("pathLabel");

        pathLayout->addWidget(pathLabel);

        projectPathLineEdit = new QLineEdit(LoadProjectDialog);
        projectPathLineEdit->setObjectName("projectPathLineEdit");

        pathLayout->addWidget(projectPathLineEdit);

        browseButton = new QPushButton(LoadProjectDialog);
        browseButton->setObjectName("browseButton");

        pathLayout->addWidget(browseButton);


        verticalLayout->addLayout(pathLayout);

        buttonBox = new QDialogButtonBox(LoadProjectDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(LoadProjectDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, LoadProjectDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, LoadProjectDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(LoadProjectDialog);
    } // setupUi

    void retranslateUi(QDialog *LoadProjectDialog)
    {
        LoadProjectDialog->setWindowTitle(QCoreApplication::translate("LoadProjectDialog", "Load from Game Project", nullptr));
        engineLabel->setText(QCoreApplication::translate("LoadProjectDialog", "Game Engine:", nullptr));
        pathLabel->setText(QCoreApplication::translate("LoadProjectDialog", "Project Path:", nullptr));
        browseButton->setText(QCoreApplication::translate("LoadProjectDialog", "Browse...", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoadProjectDialog: public Ui_LoadProjectDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOADPROJECTDIALOG_H
