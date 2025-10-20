#include "settingsdialog.h"
#include "ui_settingsdialog.h"

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

    connect(ui->googleApiRadioButton, &QRadioButton::toggled, this, &SettingsDialog::updateGoogleApiFields);
    connect(ui->googleFreeRadioButton, &QRadioButton::toggled, this, &SettingsDialog::updateGoogleApiFields);

    updateGoogleApiFields();
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

bool SettingsDialog::isGoogleApi() const
{
    return ui->googleApiRadioButton->isChecked();
}

QString SettingsDialog::llmProvider() const
{
    return ui->llmProviderComboBox->currentText();
}

QString SettingsDialog::llmApiKey() const
{
    return ui->llmApiKeyEdit->text();
}

QString SettingsDialog::llmModel() const
{
    return ui->llmModelEdit->text();
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

void SettingsDialog::setGoogleApi(bool isApi)
{
    ui->googleApiRadioButton->setChecked(isApi);
    ui->googleFreeRadioButton->setChecked(!isApi);
}

void SettingsDialog::setLlmProvider(const QString &provider)
{
    int index = ui->llmProviderComboBox->findText(provider);
    if (index != -1) {
        ui->llmProviderComboBox->setCurrentIndex(index);
    }
}

void SettingsDialog::setLlmApiKey(const QString &apiKey)
{
    ui->llmApiKeyEdit->setText(apiKey);
}

void SettingsDialog::setLlmModel(const QString &model)
{
    ui->llmModelEdit->setText(model);
}

void SettingsDialog::updateGoogleApiFields()
{
    bool isApi = ui->googleApiRadioButton->isChecked();
    ui->googleApiKeyEdit->setEnabled(isApi);
}