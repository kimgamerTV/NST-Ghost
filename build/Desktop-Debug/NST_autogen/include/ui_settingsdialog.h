/********************************************************************************
** Form generated from reading UI file 'settingsdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSDIALOG_H
#define UI_SETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SettingsDialog
{
public:
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *googleTranslateTab;
    QVBoxLayout *verticalLayout_2;
    QGroupBox *googleSettingsGroupBox;
    QFormLayout *formLayout;
    QLabel *label;
    QHBoxLayout *horizontalLayout;
    QRadioButton *googleFreeRadioButton;
    QRadioButton *googleApiRadioButton;
    QLabel *apiKeyLabel;
    QLineEdit *googleApiKeyEdit;
    QLabel *targetLanguageLabel;
    QComboBox *targetLanguageComboBox;
    QSpacerItem *verticalSpacer;
    QWidget *llmTab;
    QVBoxLayout *verticalLayout_3;
    QGroupBox *llmSettingsGroupBox;
    QFormLayout *formLayout_2;
    QLabel *llmProviderLabel;
    QComboBox *llmProviderComboBox;
    QLabel *llmApiKeyLabel;
    QLineEdit *llmApiKeyEdit;
    QLabel *llmModelLabel;
    QLineEdit *llmModelEdit;
    QSpacerItem *verticalSpacer_2;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SettingsDialog)
    {
        if (SettingsDialog->objectName().isEmpty())
            SettingsDialog->setObjectName("SettingsDialog");
        SettingsDialog->resize(480, 380);
        verticalLayout = new QVBoxLayout(SettingsDialog);
        verticalLayout->setSpacing(12);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(12, 12, 12, 12);
        tabWidget = new QTabWidget(SettingsDialog);
        tabWidget->setObjectName("tabWidget");
        googleTranslateTab = new QWidget();
        googleTranslateTab->setObjectName("googleTranslateTab");
        verticalLayout_2 = new QVBoxLayout(googleTranslateTab);
        verticalLayout_2->setSpacing(10);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(10, 10, 10, 10);
        googleSettingsGroupBox = new QGroupBox(googleTranslateTab);
        googleSettingsGroupBox->setObjectName("googleSettingsGroupBox");
        formLayout = new QFormLayout(googleSettingsGroupBox);
        formLayout->setObjectName("formLayout");
        formLayout->setHorizontalSpacing(15);
        formLayout->setVerticalSpacing(10);
        formLayout->setContentsMargins(10, 10, 10, 10);
        label = new QLabel(googleSettingsGroupBox);
        label->setObjectName("label");

        formLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, label);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(10);
        horizontalLayout->setObjectName("horizontalLayout");
        googleFreeRadioButton = new QRadioButton(googleSettingsGroupBox);
        googleFreeRadioButton->setObjectName("googleFreeRadioButton");
        googleFreeRadioButton->setChecked(true);

        horizontalLayout->addWidget(googleFreeRadioButton);

        googleApiRadioButton = new QRadioButton(googleSettingsGroupBox);
        googleApiRadioButton->setObjectName("googleApiRadioButton");

        horizontalLayout->addWidget(googleApiRadioButton);


        formLayout->setLayout(0, QFormLayout::ItemRole::FieldRole, horizontalLayout);

        apiKeyLabel = new QLabel(googleSettingsGroupBox);
        apiKeyLabel->setObjectName("apiKeyLabel");

        formLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, apiKeyLabel);

        googleApiKeyEdit = new QLineEdit(googleSettingsGroupBox);
        googleApiKeyEdit->setObjectName("googleApiKeyEdit");

        formLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, googleApiKeyEdit);

        targetLanguageLabel = new QLabel(googleSettingsGroupBox);
        targetLanguageLabel->setObjectName("targetLanguageLabel");

        formLayout->setWidget(2, QFormLayout::ItemRole::LabelRole, targetLanguageLabel);

        targetLanguageComboBox = new QComboBox(googleSettingsGroupBox);
        targetLanguageComboBox->setObjectName("targetLanguageComboBox");

        formLayout->setWidget(2, QFormLayout::ItemRole::FieldRole, targetLanguageComboBox);


        verticalLayout_2->addWidget(googleSettingsGroupBox);

        verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        tabWidget->addTab(googleTranslateTab, QString());
        llmTab = new QWidget();
        llmTab->setObjectName("llmTab");
        verticalLayout_3 = new QVBoxLayout(llmTab);
        verticalLayout_3->setSpacing(10);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setContentsMargins(10, 10, 10, 10);
        llmSettingsGroupBox = new QGroupBox(llmTab);
        llmSettingsGroupBox->setObjectName("llmSettingsGroupBox");
        formLayout_2 = new QFormLayout(llmSettingsGroupBox);
        formLayout_2->setObjectName("formLayout_2");
        formLayout_2->setHorizontalSpacing(15);
        formLayout_2->setVerticalSpacing(10);
        formLayout_2->setContentsMargins(10, 10, 10, 10);
        llmProviderLabel = new QLabel(llmSettingsGroupBox);
        llmProviderLabel->setObjectName("llmProviderLabel");

        formLayout_2->setWidget(0, QFormLayout::ItemRole::LabelRole, llmProviderLabel);

        llmProviderComboBox = new QComboBox(llmSettingsGroupBox);
        llmProviderComboBox->addItem(QString());
        llmProviderComboBox->addItem(QString());
        llmProviderComboBox->setObjectName("llmProviderComboBox");

        formLayout_2->setWidget(0, QFormLayout::ItemRole::FieldRole, llmProviderComboBox);

        llmApiKeyLabel = new QLabel(llmSettingsGroupBox);
        llmApiKeyLabel->setObjectName("llmApiKeyLabel");

        formLayout_2->setWidget(1, QFormLayout::ItemRole::LabelRole, llmApiKeyLabel);

        llmApiKeyEdit = new QLineEdit(llmSettingsGroupBox);
        llmApiKeyEdit->setObjectName("llmApiKeyEdit");

        formLayout_2->setWidget(1, QFormLayout::ItemRole::FieldRole, llmApiKeyEdit);

        llmModelLabel = new QLabel(llmSettingsGroupBox);
        llmModelLabel->setObjectName("llmModelLabel");

        formLayout_2->setWidget(2, QFormLayout::ItemRole::LabelRole, llmModelLabel);

        llmModelEdit = new QLineEdit(llmSettingsGroupBox);
        llmModelEdit->setObjectName("llmModelEdit");

        formLayout_2->setWidget(2, QFormLayout::ItemRole::FieldRole, llmModelEdit);


        verticalLayout_3->addWidget(llmSettingsGroupBox);

        verticalSpacer_2 = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_2);

        tabWidget->addTab(llmTab, QString());

        verticalLayout->addWidget(tabWidget);

        buttonBox = new QDialogButtonBox(SettingsDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Orientation::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::StandardButton::Cancel|QDialogButtonBox::StandardButton::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(SettingsDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, SettingsDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, SettingsDialog, qOverload<>(&QDialog::reject));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(SettingsDialog);
    } // setupUi

    void retranslateUi(QDialog *SettingsDialog)
    {
        SettingsDialog->setWindowTitle(QCoreApplication::translate("SettingsDialog", "Settings", nullptr));
        googleSettingsGroupBox->setTitle(QCoreApplication::translate("SettingsDialog", "Google Translate Settings", nullptr));
        label->setText(QCoreApplication::translate("SettingsDialog", "Translation Type:", nullptr));
#if QT_CONFIG(tooltip)
        googleFreeRadioButton->setToolTip(QCoreApplication::translate("SettingsDialog", "Uses an unofficial method to translate, may be unstable.", nullptr));
#endif // QT_CONFIG(tooltip)
        googleFreeRadioButton->setText(QCoreApplication::translate("SettingsDialog", "Free (Unofficial)", nullptr));
#if QT_CONFIG(tooltip)
        googleApiRadioButton->setToolTip(QCoreApplication::translate("SettingsDialog", "Uses the official Google Translate API, requires an API key.", nullptr));
#endif // QT_CONFIG(tooltip)
        googleApiRadioButton->setText(QCoreApplication::translate("SettingsDialog", "API", nullptr));
#if QT_CONFIG(tooltip)
        apiKeyLabel->setToolTip(QCoreApplication::translate("SettingsDialog", "Your Google Translate API key.", nullptr));
#endif // QT_CONFIG(tooltip)
        apiKeyLabel->setText(QCoreApplication::translate("SettingsDialog", "API Key:", nullptr));
#if QT_CONFIG(tooltip)
        targetLanguageLabel->setToolTip(QCoreApplication::translate("SettingsDialog", "The language to translate into.", nullptr));
#endif // QT_CONFIG(tooltip)
        targetLanguageLabel->setText(QCoreApplication::translate("SettingsDialog", "Target Language:", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(googleTranslateTab), QCoreApplication::translate("SettingsDialog", "Google Translate", nullptr));
        llmSettingsGroupBox->setTitle(QCoreApplication::translate("SettingsDialog", "LLM Translation Settings", nullptr));
#if QT_CONFIG(tooltip)
        llmProviderLabel->setToolTip(QCoreApplication::translate("SettingsDialog", "The LLM provider to use (e.g., OpenAI, Google AI).", nullptr));
#endif // QT_CONFIG(tooltip)
        llmProviderLabel->setText(QCoreApplication::translate("SettingsDialog", "Provider:", nullptr));
        llmProviderComboBox->setItemText(0, QCoreApplication::translate("SettingsDialog", "OpenAI", nullptr));
        llmProviderComboBox->setItemText(1, QCoreApplication::translate("SettingsDialog", "Google AI", nullptr));

#if QT_CONFIG(tooltip)
        llmApiKeyLabel->setToolTip(QCoreApplication::translate("SettingsDialog", "Your API key for the selected LLM provider.", nullptr));
#endif // QT_CONFIG(tooltip)
        llmApiKeyLabel->setText(QCoreApplication::translate("SettingsDialog", "API Key:", nullptr));
#if QT_CONFIG(tooltip)
        llmModelLabel->setToolTip(QCoreApplication::translate("SettingsDialog", "The specific LLM model to use (e.g., gpt-4, gemini-pro).", nullptr));
#endif // QT_CONFIG(tooltip)
        llmModelLabel->setText(QCoreApplication::translate("SettingsDialog", "Model:", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(llmTab), QCoreApplication::translate("SettingsDialog", "LLM Translation", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SettingsDialog: public Ui_SettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSDIALOG_H
