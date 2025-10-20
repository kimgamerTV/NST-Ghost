#include "llm_translation_service.h"
#include <QDebug>

namespace qtlingo {

LLMTranslationService::LLMTranslationService(QObject *parent)
    : ITranslationService(parent) // Pass parent to base class constructor
{
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

TranslationResult LLMTranslationService::translate(const QString &sourceText)
{
    TranslationResult result;
    result.sourceText = sourceText;
    result.translatedText = QString("[LLM Translation (%1/%2): %3]")
                              .arg(m_provider, m_model, sourceText);

    // In a real scenario, you would make an API request to an LLM here.
    // This might involve HTTP requests, or interaction with a local model.

    emit translationFinished(result);
    return result; // Return dummy result immediately
}

} // namespace qtlingo