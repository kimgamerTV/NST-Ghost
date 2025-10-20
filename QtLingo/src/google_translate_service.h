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
    TranslationResult translate(const QString &sourceText) override;

    void setApiKey(const QString &apiKey) override;
    void setTargetLanguage(const QString &language) override;
    void setGoogleTranslateMode(bool isApi) override;

private slots:
    void onNetworkReply(QNetworkReply *reply);

private:
    QNetworkAccessManager *m_networkManager;
    QString m_apiKey;
    QString m_targetLanguage;
    bool m_isApi = false;
    // Store the source text for the current translation request
    QString m_currentSourceText;
};

} // namespace qtlingo

#endif // QTLINGO_GOOGLE_TRANSLATE_SERVICE_H
