#include "translationservicemanager.h"
#include <QDebug>

#include <QVariantMap>

TranslationServiceManager::TranslationServiceManager(QObject *parent)
    : QObject(parent)
{
}

QStringList TranslationServiceManager::getAvailableServices() const
{
    return qtlingo::availableTranslationServices();
}

void TranslationServiceManager::translate(const QString &serviceName, const QString &sourceText, const QVariantMap &settings)
{
    m_currentService = qtlingo::createTranslationService(serviceName, this);
    if (m_currentService) {
        connect(m_currentService.get(), &qtlingo::ITranslationService::translationFinished, this, &TranslationServiceManager::handleTranslationFinished);
        connect(m_currentService.get(), &qtlingo::ITranslationService::errorOccurred, this, &TranslationServiceManager::handleErrorOccurred);

        if (serviceName == "Google Translate") {
            m_currentService->setTargetLanguage(settings.value("targetLanguage").toString());
            m_currentService->setGoogleTranslateMode(settings.value("googleApi").toBool());
            if (settings.value("googleApi").toBool()) {
                m_currentService->setApiKey(settings.value("googleApiKey").toString());
            }
        } else if (serviceName == "LLM Translation") {
            m_currentService->setLlmProvider(settings.value("llmProvider").toString());
            m_currentService->setApiKey(settings.value("llmApiKey").toString());
            m_currentService->setLlmModel(settings.value("llmModel").toString());
        }

        m_currentService->translate(sourceText);
    } else {
        emit errorOccurred(QString("Failed to create translation service: %1").arg(serviceName));
    }
}

void TranslationServiceManager::handleTranslationFinished(const qtlingo::TranslationResult &result)
{
    emit translationFinished(result);
}

void TranslationServiceManager::handleErrorOccurred(const QString &message)
{
    emit errorOccurred(message);
}
