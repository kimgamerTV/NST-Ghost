#include "google_translate_service.h"
#include <QDebug>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRegularExpression>

namespace qtlingo {

GoogleTranslateService::GoogleTranslateService(QObject *parent)
    : ITranslationService(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{
    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &GoogleTranslateService::onNetworkReply);
}

void GoogleTranslateService::setApiKey(const QString &apiKey)
{
    m_apiKey = apiKey;
}

void GoogleTranslateService::setTargetLanguage(const QString &language)
{
    m_targetLanguage = language;
}

void GoogleTranslateService::setSourceLanguage(const QString &language)
{
    m_sourceLanguage = language;
}

void GoogleTranslateService::setGoogleTranslateMode(bool isApi)
{
    m_isApi = isApi;
}

void GoogleTranslateService::translate(const QString &sourceText)
{
    m_currentSourceText = sourceText;

    if (m_isApi) {
        translateWithApi(sourceText);
    } else {
        translateWithFreeApi(sourceText);
    }
}

void GoogleTranslateService::translateWithApi(const QString &sourceText)
{
    if (m_apiKey.isEmpty()) {
        emit errorOccurred("API key is not set for Google Translate.");
        return;
    }

    QUrl url("https://translation.googleapis.com/language/translate/v2");
    QUrlQuery query;
    query.addQueryItem("q", sourceText);
    query.addQueryItem("target", m_targetLanguage);
    query.addQueryItem("key", m_apiKey);
    query.addQueryItem("format", "text");

    if (m_sourceLanguage != "auto") {
        query.addQueryItem("source", m_sourceLanguage);
    }

    url.setQuery(query);
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::User, QVariant(true)); // Mark as API request
    m_networkManager->get(request);
}

void GoogleTranslateService::translateWithFreeApi(const QString &sourceText)
{
    // Use Google Translate's free web interface
    QString urlString = QString("https://translate.googleapis.com/translate_a/single?client=gtx&sl=%1&tl=%2&dt=t&q=%3")
                        .arg(m_sourceLanguage)
                        .arg(m_targetLanguage)
                        .arg(QString(QUrl::toPercentEncoding(sourceText)));

    QUrl url(urlString);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader,
                     "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36");
    request.setAttribute(QNetworkRequest::User, QVariant(false)); // Mark as free request

    m_networkManager->get(request);
}

QString GoogleTranslateService::extractTranslationFromHtml(const QString &html)
{
    // Try to parse JSON response from free API
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(html.toUtf8(), &parseError);

    if (parseError.error == QJsonParseError::NoError && jsonDoc.isArray()) {
        QJsonArray mainArray = jsonDoc.array();
        if (mainArray.size() > 0 && mainArray[0].isArray()) {
            QJsonArray translationArray = mainArray[0].toArray();
            QString result;

            // Concatenate all translation parts
            for (const QJsonValue &value : translationArray) {
                if (value.isArray()) {
                    QJsonArray partArray = value.toArray();
                    if (partArray.size() > 0) {
                        result += partArray[0].toString();
                    }
                }
            }

            return result;
        }
    }

    return "[Error: Could not parse translation]";
}

void GoogleTranslateService::onNetworkReply(QNetworkReply *reply)
{
    TranslationResult result;
    result.sourceText = m_currentSourceText;

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        bool isApiRequest = reply->request().attribute(QNetworkRequest::User).toBool();

        if (isApiRequest) {
            // Parse official API response
            qDebug() << "Google Translate API Response:" << responseData;

            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
            QJsonObject jsonObj = jsonDoc.object();
            QJsonObject dataObj = jsonObj["data"].toObject();
            QJsonArray translations = dataObj["translations"].toArray();

            if (!translations.isEmpty()) {
                result.translatedText = translations[0].toObject()["translatedText"].toString();
            } else {
                result.translatedText = "[Error: No translation found]";
                emit errorOccurred("No translation found in API response");
            }
        } else {
            // Parse free API response
            qDebug() << "Google Translate Free API Response:" << responseData;
            result.translatedText = extractTranslationFromHtml(QString::fromUtf8(responseData));
        }

        emit translationFinished(result);
    } else {
        qDebug() << "Network Error:" << reply->errorString();
        emit errorOccurred(reply->errorString());
    }

    reply->deleteLater();
}

} // namespace qtlingo
