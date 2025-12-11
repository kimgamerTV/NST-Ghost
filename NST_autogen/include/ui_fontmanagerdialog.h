/********************************************************************************
** Form generated from reading UI file 'fontmanagerdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FONTMANAGERDIALOG_H
#define UI_FONTMANAGERDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_FontManagerDialog
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *fontGroupBox;
    QGridLayout *gridLayout;
    QLabel *fontListLabel;
    QListView *fontListView;
    QVBoxLayout *verticalLayout_2;
    QPushButton *addButton;
    QPushButton *removeButton;
    QPushButton *replaceButton;
    QSpacerItem *verticalSpacer;
    QGroupBox *previewGroupBox;
    QVBoxLayout *verticalLayout_3;
    QLabel *fontPreviewLabel;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *FontManagerDialog)
    {
        if (FontManagerDialog->objectName().isEmpty())
            FontManagerDialog->setObjectName("FontManagerDialog");
        FontManagerDialog->resize(520, 420);
        verticalLayout = new QVBoxLayout(FontManagerDialog);
        verticalLayout->setObjectName("verticalLayout");
        fontGroupBox = new QGroupBox(FontManagerDialog);
        fontGroupBox->setObjectName("fontGroupBox");
        gridLayout = new QGridLayout(fontGroupBox);
        gridLayout->setObjectName("gridLayout");
        fontListLabel = new QLabel(fontGroupBox);
        fontListLabel->setObjectName("fontListLabel");

        gridLayout->addWidget(fontListLabel, 0, 0, 1, 1);

        fontListView = new QListView(fontGroupBox);
        fontListView->setObjectName("fontListView");

        gridLayout->addWidget(fontListView, 1, 0, 1, 1);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        addButton = new QPushButton(fontGroupBox);
        addButton->setObjectName("addButton");

        verticalLayout_2->addWidget(addButton);

        removeButton = new QPushButton(fontGroupBox);
        removeButton->setObjectName("removeButton");

        verticalLayout_2->addWidget(removeButton);

        replaceButton = new QPushButton(fontGroupBox);
        replaceButton->setObjectName("replaceButton");

        verticalLayout_2->addWidget(replaceButton);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);


        gridLayout->addLayout(verticalLayout_2, 1, 1, 1, 1);

        previewGroupBox = new QGroupBox(fontGroupBox);
        previewGroupBox->setObjectName("previewGroupBox");
        verticalLayout_3 = new QVBoxLayout(previewGroupBox);
        verticalLayout_3->setObjectName("verticalLayout_3");
        fontPreviewLabel = new QLabel(previewGroupBox);
        fontPreviewLabel->setObjectName("fontPreviewLabel");
        fontPreviewLabel->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(fontPreviewLabel);


        gridLayout->addWidget(previewGroupBox, 2, 0, 1, 2);


        verticalLayout->addWidget(fontGroupBox);

        buttonBox = new QDialogButtonBox(FontManagerDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(FontManagerDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, FontManagerDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, FontManagerDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(FontManagerDialog);
    } // setupUi

    void retranslateUi(QDialog *FontManagerDialog)
    {
        FontManagerDialog->setWindowTitle(QCoreApplication::translate("FontManagerDialog", "Font Manager", nullptr));
        fontGroupBox->setTitle(QCoreApplication::translate("FontManagerDialog", "Font Management", nullptr));
        fontListLabel->setText(QCoreApplication::translate("FontManagerDialog", "Game Fonts:", nullptr));
        addButton->setText(QCoreApplication::translate("FontManagerDialog", "Add...", nullptr));
        removeButton->setText(QCoreApplication::translate("FontManagerDialog", "Remove", nullptr));
        replaceButton->setText(QCoreApplication::translate("FontManagerDialog", "Replace...", nullptr));
        previewGroupBox->setTitle(QCoreApplication::translate("FontManagerDialog", "Preview", nullptr));
        fontPreviewLabel->setText(QCoreApplication::translate("FontManagerDialog", "The quick brown fox jumps over the lazy dog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class FontManagerDialog: public Ui_FontManagerDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FONTMANAGERDIALOG_H
