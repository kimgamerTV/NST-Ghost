#include "google_translate_service.h"
#include <QDebug>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace qtlingo {

GoogleTranslateService::GoogleTranslateService(QObject *parent)
    : ITranslationService(parent) // Pass parent to base class constructor
    , m_networkManager(new QNetworkAccessManager(this))
{
    connect(m_networkManager, &QNetworkAccessManager::finished, this, &GoogleTranslateService::onNetworkReply);
}

void GoogleTranslateService::setApiKey(const QString &apiKey)
{
    m_apiKey = apiKey;
}

void GoogleTranslateService::setTargetLanguage(const QString &language)
{
    m_targetLanguage = language;
}

void GoogleTranslateService::setGoogleTranslateMode(bool isApi)
{
    m_isApi = isApi;
}

TranslationResult GoogleTranslateService::translate(const QString &sourceText)
{
    m_currentSourceText = sourceText;

    if (!m_isApi) {
        TranslationResult result;
        result.sourceText = sourceText;
        result.translatedText = "[Google Translate (Free): " + sourceText + "]";
        emit translationFinished(result);
        return result;
    }

    if (m_apiKey.isEmpty()) {
        TranslationResult result;
        result.sourceText = sourceText;
        result.translatedText = "[Error: API key is not set]";
        emit errorOccurred("API key is not set for Google Translate.");
        return result;
    }

    QUrl url("https://translation.googleapis.com/language/translate/v2");
    QUrlQuery query;
    query.addQueryItem("q", sourceText);
    query.addQueryItem("target", m_targetLanguage);
    query.addQueryItem("key", m_apiKey);
    query.addQueryItem("format", "text");
    url.setQuery(query);
    QNetworkRequest request(url);
    m_networkManager->get(request);

    // Return a temporary result, the actual result will be emitted when the network reply is finished
    TranslationResult result;
    result.sourceText = sourceText;
    result.translatedText = "[Translating...]";
    return result;
}

void GoogleTranslateService::onNetworkReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        qDebug() << "Google Translate API Response:" << responseData;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        QJsonObject jsonObj = jsonDoc.object();
        QJsonObject dataObj = jsonObj["data"].toObject();
        QJsonArray translations = dataObj["translations"].toArray();
        QString translatedText = translations[0].toObject()["translatedText"].toString();

        TranslationResult result;
        result.sourceText = m_currentSourceText;
        result.translatedText = translatedText;
        emit translationFinished(result);
    } else {
        emit errorOccurred(reply->errorString());
    }
    reply->deleteLater();
}

} // namespace qtlingo