#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QMap>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    // Populate with some languages
    ui->targetLanguageComboBox->addItem("English", "en");
    ui->targetLanguageComboBox->addItem("Spanish", "es");
    ui->targetLanguageComboBox->addItem("French", "fr");
    ui->targetLanguageComboBox->addItem("German", "de");
    ui->targetLanguageComboBox->addItem("Italian", "it");
    ui->targetLanguageComboBox->addItem("Portuguese", "pt");
    ui->targetLanguageComboBox->addItem("Russian", "ru");
    ui->targetLanguageComboBox->addItem("Chinese (Simplified)", "zh-CN");
    ui->targetLanguageComboBox->addItem("Japanese", "ja");
    ui->targetLanguageComboBox->addItem("Korean", "ko");
    ui->targetLanguageComboBox->addItem("Arabic", "ar");
    ui->targetLanguageComboBox->addItem("Hindi", "hi");
    ui->targetLanguageComboBox->addItem("Thai", "th");

    connect(ui->quickTranslateRadioButton, &QRadioButton::toggled, this, &SettingsDialog::updateConfigPanel);
    connect(ui->professionalTranslateRadioButton, &QRadioButton::toggled, this, &SettingsDialog::updateConfigPanel);
    connect(ui->aiPoweredTranslateRadioButton, &QRadioButton::toggled, this, &SettingsDialog::updateConfigPanel);
    connect(ui->llmProviderComboBox, &QComboBox::currentIndexChanged, this, &SettingsDialog::updateLlmModelComboBox);

    updateConfigPanel();
    updateLlmModelComboBox();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

QString SettingsDialog::googleApiKey() const
{
    return ui->googleApiKeyEdit->text();
}

QString SettingsDialog::targetLanguage() const
{
    return ui->targetLanguageComboBox->currentData().toString();
}

QString SettingsDialog::targetLanguageName() const
{
    return ui->targetLanguageComboBox->currentText();
}

QString SettingsDialog::llmProvider() const
{
    QString provider = ui->llmProviderComboBox->currentText();
    if (provider == "Google") return "Google";
    return provider;
}

QString SettingsDialog::llmApiKey() const
{
    return ui->llmApiKeyEdit->text();
}

QString SettingsDialog::llmBaseUrl() const
{
    return ui->llmBaseUrlEdit->text();
}

QString SettingsDialog::llmModel() const
{
    return ui->llmModelComboBox->currentText();
}

void SettingsDialog::setGoogleApiKey(const QString &apiKey)
{
    ui->googleApiKeyEdit->setText(apiKey);
}

void SettingsDialog::setTargetLanguage(const QString &language)
{
    int index = ui->targetLanguageComboBox->findData(language);
    if (index != -1) {
        ui->targetLanguageComboBox->setCurrentIndex(index);
    }
}

void SettingsDialog::setLlmProvider(const QString &provider)
{
    QString providerText = provider;
    if (provider == "Google") providerText = "Google AI";
    int index = ui->llmProviderComboBox->findText(providerText);
    if (index != -1) {
        ui->llmProviderComboBox->setCurrentIndex(index);
    }
}

void SettingsDialog::setLlmApiKey(const QString &apiKey)
{
    ui->llmApiKeyEdit->setText(apiKey);
}

void SettingsDialog::setLlmBaseUrl(const QString &baseUrl)
{
    ui->llmBaseUrlEdit->setText(baseUrl);
}

void SettingsDialog::setLlmModel(const QString &model)
{
    updateLlmModelComboBox(); // Ensure the models are populated for the current provider
    if (ui->llmModelComboBox->isEditable()) {
        ui->llmModelComboBox->setCurrentText(model);
    } else {
        int index = ui->llmModelComboBox->findText(model);
        if (index != -1) {
            ui->llmModelComboBox->setCurrentIndex(index);
        }
    }
}

bool SettingsDialog::isGoogleApi() const
{
    return ui->professionalTranslateRadioButton->isChecked();
}

void SettingsDialog::setGoogleApi(bool isApi)
{
    ui->professionalTranslateRadioButton->setChecked(isApi);
    ui->quickTranslateRadioButton->setChecked(!isApi);
}

void SettingsDialog::updateConfigPanel()
{
    if (ui->quickTranslateRadioButton->isChecked()) {
        ui->configStackedWidget->setCurrentIndex(0); // Page for Quick Translate
    } else if (ui->professionalTranslateRadioButton->isChecked()) {
        ui->configStackedWidget->setCurrentIndex(1); // Page for Professional (Google API)
    } else if (ui->aiPoweredTranslateRadioButton->isChecked()) {
        ui->configStackedWidget->setCurrentIndex(2); // Page for AI-Powered (LLM)
    }
}

void SettingsDialog::updateLlmModelComboBox()
{
    QString provider = ui->llmProviderComboBox->currentText();
    ui->llmModelComboBox->clear();

    if (provider == "OpenAI") {
        ui->llmModelComboBox->addItems({"gpt-4o", "gpt-4-turbo", "gpt-4", "gpt-3.5-turbo"});
    } else if (provider == "Anthropic") {
        ui->llmModelComboBox->addItems({"claude-3-opus-20240229", "claude-3-sonnet-20240229", "claude-3-haiku-20240307"});
    } else if (provider == "Google AI") {
        ui->llmModelComboBox->addItems({"gemini-1.5-pro-latest", "gemini-pro"});
    }
}