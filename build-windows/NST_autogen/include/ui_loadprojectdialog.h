/********************************************************************************
** Form generated from reading UI file 'loadprojectdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOADPROJECTDIALOG_H
#define UI_LOADPROJECTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LoadProjectDialog
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *contentWidget;
    QHBoxLayout *pathLayout;
    QLineEdit *projectPathLineEdit;
    QPushButton *browseButton;
    QHBoxLayout *buttonLayout;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *LoadProjectDialog)
    {
        if (LoadProjectDialog->objectName().isEmpty())
            LoadProjectDialog->setObjectName("LoadProjectDialog");
        LoadProjectDialog->resize(700, 500);
        verticalLayout = new QVBoxLayout(LoadProjectDialog);
        verticalLayout->setSpacing(15);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 20);
        contentWidget = new QWidget(LoadProjectDialog);
        contentWidget->setObjectName("contentWidget");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(contentWidget->sizePolicy().hasHeightForWidth());
        contentWidget->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(contentWidget);

        pathLayout = new QHBoxLayout();
        pathLayout->setSpacing(10);
        pathLayout->setObjectName("pathLayout");
        pathLayout->setContentsMargins(30, -1, 30, -1);
        projectPathLineEdit = new QLineEdit(LoadProjectDialog);
        projectPathLineEdit->setObjectName("projectPathLineEdit");

        pathLayout->addWidget(projectPathLineEdit);

        browseButton = new QPushButton(LoadProjectDialog);
        browseButton->setObjectName("browseButton");

        pathLayout->addWidget(browseButton);


        verticalLayout->addLayout(pathLayout);

        buttonLayout = new QHBoxLayout();
        buttonLayout->setObjectName("buttonLayout");
        buttonLayout->setContentsMargins(30, -1, 30, -1);
        buttonBox = new QDialogButtonBox(LoadProjectDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        buttonLayout->addWidget(buttonBox);


        verticalLayout->addLayout(buttonLayout);


        retranslateUi(LoadProjectDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, LoadProjectDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, LoadProjectDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(LoadProjectDialog);
    } // setupUi

    void retranslateUi(QDialog *LoadProjectDialog)
    {
        LoadProjectDialog->setWindowTitle(QCoreApplication::translate("LoadProjectDialog", "Load from Game Project", nullptr));
        browseButton->setText(QCoreApplication::translate("LoadProjectDialog", "Browse...", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoadProjectDialog: public Ui_LoadProjectDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOADPROJECTDIALOG_H
