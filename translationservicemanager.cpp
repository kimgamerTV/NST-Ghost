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
    // Create the service with 'this' as parent, so Qt manages its lifetime
    std::unique_ptr<qtlingo::ITranslationService> uniqueService = qtlingo::createTranslationService(serviceName, this);
    if (uniqueService) {
        qtlingo::ITranslationService* service = uniqueService.release(); // Transfer ownership to Qt's parent-child system
        m_activeServices.append(service); // Add QPointer to the list of active services

        connect(service, &qtlingo::ITranslationService::translationFinished, this, &TranslationServiceManager::handleTranslationFinished);
        connect(service, &qtlingo::ITranslationService::errorOccurred, this, &TranslationServiceManager::handleErrorOccurred);

        if (serviceName == "Google Translate") {
            service->setTargetLanguage(settings.value("targetLanguage").toString());
            service->setGoogleTranslateMode(settings.value("googleApi").toBool());
            if (settings.value("googleApi").toBool()) {
                service->setApiKey(settings.value("googleApiKey").toString());
            }
        } else if (serviceName == "LLM Translation") {
            service->setLlmProvider(settings.value("llmProvider").toString());
            service->setApiKey(settings.value("llmApiKey").toString());
            service->setLlmModel(settings.value("llmModel").toString());
            service->setTargetLanguage(settings.value("targetLanguage").toString());
        }

        service->translate(sourceText);
    } else {
        emit errorOccurred(QString("Failed to create translation service: %1").arg(serviceName));
    }
}

void TranslationServiceManager::handleTranslationFinished(const qtlingo::TranslationResult &result)
{
    // Find the service that emitted the signal
    qtlingo::ITranslationService* service = qobject_cast<qtlingo::ITranslationService*>(sender());
    if (service) {
        // Find the corresponding unique_ptr in m_activeServices and remove it
        for (int i = 0; i < m_activeServices.size(); ++i) {
            if (m_activeServices.at(i).get() == service) {
                m_activeServices.removeAt(i);
                break;
            }
        }
    }
    emit translationFinished(result);
}

void TranslationServiceManager::handleErrorOccurred(const QString &message)
{
    qtlingo::ITranslationService* service = qobject_cast<qtlingo::ITranslationService*>(sender());
    if (service) {
        for (int i = 0; i < m_activeServices.size(); ++i) {
            if (m_activeServices.at(i).get() == service) {
                m_activeServices.removeAt(i);
                break;
            }
        }
    }
    emit errorOccurred(message);
}
