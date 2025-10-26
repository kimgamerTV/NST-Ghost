#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class SettingsDialog;
}
QT_END_NAMESPACE

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

    QString googleApiKey() const;
    QString targetLanguage() const;
    QString targetLanguageName() const;
    bool isGoogleApi() const;
    QString llmProvider() const;
    QString llmApiKey() const;
    QString llmModel() const;
    QString llmBaseUrl() const;

    void setGoogleApiKey(const QString &apiKey);
    void setTargetLanguage(const QString &language);
    void setGoogleApi(bool isApi);
    void setLlmProvider(const QString &provider);
    void setLlmApiKey(const QString &apiKey);
    void setLlmModel(const QString &model);
    void setLlmBaseUrl(const QString &baseUrl);

private slots:
    void updateConfigPanel();
    void updateLlmModelComboBox();

private:
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
