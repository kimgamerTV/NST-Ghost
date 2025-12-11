/********************************************************************************
** Form generated from reading UI file 'settingsdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
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
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SettingsDialog
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *translationModeGroupBox;
    QHBoxLayout *horizontalLayout_mode;
    QRadioButton *quickTranslateRadioButton;
    QRadioButton *professionalTranslateRadioButton;
    QRadioButton *aiPoweredTranslateRadioButton;
    QGroupBox *basicSettingsGroupBox;
    QFormLayout *formLayout_basic;
    QLabel *targetLanguageLabel;
    QComboBox *targetLanguageComboBox;
    QStackedWidget *configStackedWidget;
    QWidget *page_quick;
    QVBoxLayout *verticalLayout_quick;
    QLabel *label_quick_info;
    QWidget *page_professional;
    QFormLayout *formLayout_professional;
    QLabel *googleApiKeyLabel;
    QLineEdit *googleApiKeyEdit;
    QWidget *page_ai_powered;
    QFormLayout *formLayout_ai_powered;
    QLabel *llmProviderLabel;
    QComboBox *llmProviderComboBox;
    QLabel *llmApiKeyLabel;
    QLineEdit *llmApiKeyEdit;
    QLabel *llmModelLabel;
    QComboBox *llmModelComboBox;
    QGroupBox *llmAdvancedGroupBox;
    QFormLayout *formLayout_advanced;
    QLabel *llmBaseUrlLabel;
    QLineEdit *llmBaseUrlEdit;
    QSpacerItem *verticalSpacer_bottom;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SettingsDialog)
    {
        if (SettingsDialog->objectName().isEmpty())
            SettingsDialog->setObjectName("SettingsDialog");
        SettingsDialog->resize(480, 420);
        verticalLayout = new QVBoxLayout(SettingsDialog);
        verticalLayout->setSpacing(12);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(12, 12, 12, 12);
        translationModeGroupBox = new QGroupBox(SettingsDialog);
        translationModeGroupBox->setObjectName("translationModeGroupBox");
        horizontalLayout_mode = new QHBoxLayout(translationModeGroupBox);
        horizontalLayout_mode->setObjectName("horizontalLayout_mode");
        quickTranslateRadioButton = new QRadioButton(translationModeGroupBox);
        quickTranslateRadioButton->setObjectName("quickTranslateRadioButton");
        quickTranslateRadioButton->setChecked(true);

        horizontalLayout_mode->addWidget(quickTranslateRadioButton);

        professionalTranslateRadioButton = new QRadioButton(translationModeGroupBox);
        professionalTranslateRadioButton->setObjectName("professionalTranslateRadioButton");

        horizontalLayout_mode->addWidget(professionalTranslateRadioButton);

        aiPoweredTranslateRadioButton = new QRadioButton(translationModeGroupBox);
        aiPoweredTranslateRadioButton->setObjectName("aiPoweredTranslateRadioButton");

        horizontalLayout_mode->addWidget(aiPoweredTranslateRadioButton);


        verticalLayout->addWidget(translationModeGroupBox);

        basicSettingsGroupBox = new QGroupBox(SettingsDialog);
        basicSettingsGroupBox->setObjectName("basicSettingsGroupBox");
        formLayout_basic = new QFormLayout(basicSettingsGroupBox);
        formLayout_basic->setObjectName("formLayout_basic");
        formLayout_basic->setHorizontalSpacing(15);
        formLayout_basic->setVerticalSpacing(10);
        formLayout_basic->setContentsMargins(10, 10, 10, 10);
        targetLanguageLabel = new QLabel(basicSettingsGroupBox);
        targetLanguageLabel->setObjectName("targetLanguageLabel");

        formLayout_basic->setWidget(0, QFormLayout::ItemRole::LabelRole, targetLanguageLabel);

        targetLanguageComboBox = new QComboBox(basicSettingsGroupBox);
        targetLanguageComboBox->setObjectName("targetLanguageComboBox");

        formLayout_basic->setWidget(0, QFormLayout::ItemRole::FieldRole, targetLanguageComboBox);


        verticalLayout->addWidget(basicSettingsGroupBox);

        configStackedWidget = new QStackedWidget(SettingsDialog);
        configStackedWidget->setObjectName("configStackedWidget");
        page_quick = new QWidget();
        page_quick->setObjectName("page_quick");
        verticalLayout_quick = new QVBoxLayout(page_quick);
        verticalLayout_quick->setObjectName("verticalLayout_quick");
        label_quick_info = new QLabel(page_quick);
        label_quick_info->setObjectName("label_quick_info");
        label_quick_info->setAlignment(Qt::AlignCenter);

        verticalLayout_quick->addWidget(label_quick_info);

        configStackedWidget->addWidget(page_quick);
        page_professional = new QWidget();
        page_professional->setObjectName("page_professional");
        formLayout_professional = new QFormLayout(page_professional);
        formLayout_professional->setObjectName("formLayout_professional");
        formLayout_professional->setHorizontalSpacing(15);
        formLayout_professional->setVerticalSpacing(10);
        formLayout_professional->setContentsMargins(10, 10, 10, 10);
        googleApiKeyLabel = new QLabel(page_professional);
        googleApiKeyLabel->setObjectName("googleApiKeyLabel");

        formLayout_professional->setWidget(0, QFormLayout::ItemRole::LabelRole, googleApiKeyLabel);

        googleApiKeyEdit = new QLineEdit(page_professional);
        googleApiKeyEdit->setObjectName("googleApiKeyEdit");

        formLayout_professional->setWidget(0, QFormLayout::ItemRole::FieldRole, googleApiKeyEdit);

        configStackedWidget->addWidget(page_professional);
        page_ai_powered = new QWidget();
        page_ai_powered->setObjectName("page_ai_powered");
        formLayout_ai_powered = new QFormLayout(page_ai_powered);
        formLayout_ai_powered->setObjectName("formLayout_ai_powered");
        formLayout_ai_powered->setHorizontalSpacing(15);
        formLayout_ai_powered->setVerticalSpacing(10);
        formLayout_ai_powered->setContentsMargins(10, 10, 10, 10);
        llmProviderLabel = new QLabel(page_ai_powered);
        llmProviderLabel->setObjectName("llmProviderLabel");

        formLayout_ai_powered->setWidget(0, QFormLayout::ItemRole::LabelRole, llmProviderLabel);

        llmProviderComboBox = new QComboBox(page_ai_powered);
        llmProviderComboBox->addItem(QString());
        llmProviderComboBox->addItem(QString());
        llmProviderComboBox->setObjectName("llmProviderComboBox");

        formLayout_ai_powered->setWidget(0, QFormLayout::ItemRole::FieldRole, llmProviderComboBox);

        llmApiKeyLabel = new QLabel(page_ai_powered);
        llmApiKeyLabel->setObjectName("llmApiKeyLabel");

        formLayout_ai_powered->setWidget(1, QFormLayout::ItemRole::LabelRole, llmApiKeyLabel);

        llmApiKeyEdit = new QLineEdit(page_ai_powered);
        llmApiKeyEdit->setObjectName("llmApiKeyEdit");

        formLayout_ai_powered->setWidget(1, QFormLayout::ItemRole::FieldRole, llmApiKeyEdit);

        llmModelLabel = new QLabel(page_ai_powered);
        llmModelLabel->setObjectName("llmModelLabel");

        formLayout_ai_powered->setWidget(2, QFormLayout::ItemRole::LabelRole, llmModelLabel);

        llmModelComboBox = new QComboBox(page_ai_powered);
        llmModelComboBox->setObjectName("llmModelComboBox");

        formLayout_ai_powered->setWidget(2, QFormLayout::ItemRole::FieldRole, llmModelComboBox);

        llmAdvancedGroupBox = new QGroupBox(page_ai_powered);
        llmAdvancedGroupBox->setObjectName("llmAdvancedGroupBox");
        llmAdvancedGroupBox->setCheckable(true);
        llmAdvancedGroupBox->setChecked(false);
        formLayout_advanced = new QFormLayout(llmAdvancedGroupBox);
        formLayout_advanced->setObjectName("formLayout_advanced");
        llmBaseUrlLabel = new QLabel(llmAdvancedGroupBox);
        llmBaseUrlLabel->setObjectName("llmBaseUrlLabel");

        formLayout_advanced->setWidget(0, QFormLayout::ItemRole::LabelRole, llmBaseUrlLabel);

        llmBaseUrlEdit = new QLineEdit(llmAdvancedGroupBox);
        llmBaseUrlEdit->setObjectName("llmBaseUrlEdit");

        formLayout_advanced->setWidget(0, QFormLayout::ItemRole::FieldRole, llmBaseUrlEdit);


        formLayout_ai_powered->setWidget(3, QFormLayout::ItemRole::SpanningRole, llmAdvancedGroupBox);

        configStackedWidget->addWidget(page_ai_powered);

        verticalLayout->addWidget(configStackedWidget);

        verticalSpacer_bottom = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer_bottom);

        buttonBox = new QDialogButtonBox(SettingsDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Orientation::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::StandardButton::Cancel|QDialogButtonBox::StandardButton::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(SettingsDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, SettingsDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, SettingsDialog, qOverload<>(&QDialog::reject));

        configStackedWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(SettingsDialog);
    } // setupUi

    void retranslateUi(QDialog *SettingsDialog)
    {
        SettingsDialog->setWindowTitle(QCoreApplication::translate("SettingsDialog", "Settings", nullptr));
        translationModeGroupBox->setTitle(QCoreApplication::translate("SettingsDialog", "Translation Mode", nullptr));
        quickTranslateRadioButton->setText(QCoreApplication::translate("SettingsDialog", "\360\237\206\223 Quick (Google Free)", nullptr));
        professionalTranslateRadioButton->setText(QCoreApplication::translate("SettingsDialog", "\360\237\224\221 Professional (Google API)", nullptr));
        aiPoweredTranslateRadioButton->setText(QCoreApplication::translate("SettingsDialog", "\360\237\244\226 AI-Powered (LLM)", nullptr));
        basicSettingsGroupBox->setTitle(QCoreApplication::translate("SettingsDialog", "Basic Settings", nullptr));
#if QT_CONFIG(tooltip)
        targetLanguageLabel->setToolTip(QCoreApplication::translate("SettingsDialog", "The language to translate into.", nullptr));
#endif // QT_CONFIG(tooltip)
        targetLanguageLabel->setText(QCoreApplication::translate("SettingsDialog", "Target Language:", nullptr));
        label_quick_info->setText(QCoreApplication::translate("SettingsDialog", "No specific settings for Quick Translate.", nullptr));
#if QT_CONFIG(tooltip)
        googleApiKeyLabel->setToolTip(QCoreApplication::translate("SettingsDialog", "Your Google Translate API key.", nullptr));
#endif // QT_CONFIG(tooltip)
        googleApiKeyLabel->setText(QCoreApplication::translate("SettingsDialog", "API Key:", nullptr));
#if QT_CONFIG(tooltip)
        llmProviderLabel->setToolTip(QCoreApplication::translate("SettingsDialog", "The LLM provider to use (e.g., OpenAI, Google AI).", nullptr));
#endif // QT_CONFIG(tooltip)
        llmProviderLabel->setText(QCoreApplication::translate("SettingsDialog", "Provider:", nullptr));
        llmProviderComboBox->setItemText(0, QCoreApplication::translate("SettingsDialog", "Google AI", nullptr));
        llmProviderComboBox->setItemText(1, QCoreApplication::translate("SettingsDialog", "Anthropic", nullptr));

#if QT_CONFIG(tooltip)
        llmApiKeyLabel->setToolTip(QCoreApplication::translate("SettingsDialog", "Your API key for the selected LLM provider.", nullptr));
#endif // QT_CONFIG(tooltip)
        llmApiKeyLabel->setText(QCoreApplication::translate("SettingsDialog", "API Key:", nullptr));
#if QT_CONFIG(tooltip)
        llmModelLabel->setToolTip(QCoreApplication::translate("SettingsDialog", "The specific LLM model to use.", nullptr));
#endif // QT_CONFIG(tooltip)
        llmModelLabel->setText(QCoreApplication::translate("SettingsDialog", "Model:", nullptr));
        llmAdvancedGroupBox->setTitle(QCoreApplication::translate("SettingsDialog", "Advanced", nullptr));
        llmBaseUrlLabel->setText(QCoreApplication::translate("SettingsDialog", "Base URL:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SettingsDialog: public Ui_SettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSDIALOG_H
