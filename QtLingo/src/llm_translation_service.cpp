#include "llm_translation_service.h"
#include <QDebug>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace qtlingo {

LLMTranslationService::LLMTranslationService(QObject *parent)
    : ITranslationService(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{
    connect(m_networkManager, &QNetworkAccessManager::finished, this, &LLMTranslationService::onNetworkReply);
}

void LLMTranslationService::setApiKey(const QString &apiKey)
{
    m_apiKey = apiKey;
}

void LLMTranslationService::setLlmProvider(const QString &provider)
{
    m_provider = provider;
}

void LLMTranslationService::setLlmModel(const QString &model)
{
    m_model = model;
}

void LLMTranslationService::setTargetLanguage(const QString &language)
{
    m_targetLanguage = language;
}

TranslationResult LLMTranslationService::translate(const QString &sourceText)
{
    m_currentSourceText = sourceText;

    if (m_apiKey.isEmpty()) {
        TranslationResult result;
        result.sourceText = sourceText;
        result.translatedText = "[Error: API key is not set]";
        emit errorOccurred("API key is not set for LLM Translation.");
        return result;
    }

    QJsonObject requestBody;
    requestBody["model"] = m_model;
    QJsonArray messages;
    QJsonObject message;
    message["role"] = "user";
    message["content"] = QString("Translate the following text to %1: %2").arg(m_targetLanguage, sourceText);
    messages.append(message);
    requestBody["messages"] = messages;

    QNetworkRequest request(QUrl("https://api.openai.com/v1/chat/completions"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + m_apiKey).toUtf8());

    m_networkManager->post(request, QJsonDocument(requestBody).toJson());

    // Return a temporary result, the actual result will be emitted when the network reply is finished
    TranslationResult result;
    result.sourceText = sourceText;
    result.translatedText = "[Translating...]";
    return result;
}

void LLMTranslationService::onNetworkReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        qDebug() << "LLM API Response:" << responseData;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        QJsonObject jsonObj = jsonDoc.object();
        QJsonArray choices = jsonObj["choices"].toArray();
        QString translatedText = choices[0].toObject()["message"].toObject()["content"].toString();

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