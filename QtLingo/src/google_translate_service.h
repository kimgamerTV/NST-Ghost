#ifndef QTLINGO_GOOGLE_TRANSLATE_SERVICE_H
#define QTLINGO_GOOGLE_TRANSLATE_SERVICE_H

#include "qtlingo/translationservice.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace qtlingo {

class GoogleTranslateService : public ITranslationService {
    Q_OBJECT
public:
    GoogleTranslateService(QObject *parent = nullptr);
    QString serviceName() const override { return "Google Translate"; }
    void translate(const QString &sourceText) override;

    // Batch Translation
    bool supportsBatchTranslation() const override { return true; }
    void batchTranslate(const QStringList &sourceTexts) override;

    void setApiKey(const QString &apiKey) override;
    void setTargetLanguage(const QString &language) override;
    void setSourceLanguage(const QString &language);
    void setGoogleTranslateMode(bool isApi) override;

private slots:
    void onNetworkReply(QNetworkReply *reply);

private:
    void translateWithApi(const QString &sourceText);
    void translateWithFreeApi(const QString &sourceText);
    QString extractTranslationFromHtml(const QString &html);

    QNetworkAccessManager *m_networkManager;
    QString m_apiKey;
    QString m_targetLanguage;
    QString m_sourceLanguage = "auto"; // Default to auto-detect
    bool m_isApi = false;
    QString m_currentSourceText;
    QStringList m_currentBatchSource;
};

} // namespace qtlingo

#endif // QTLINGO_GOOGLE_TRANSLATE_SERVICE_H
