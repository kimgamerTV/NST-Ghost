#include "translationservicemanager.h"
#include <QDebug>

TranslationServiceManager::TranslationServiceManager(QObject *parent)
    : QObject(parent)
{
}

TranslationServiceManager::~TranslationServiceManager()
{
    qDeleteAll(m_services);
}

QStringList TranslationServiceManager::getAvailableServices() const
{
    return qtlingo::availableTranslationServices();
}

void TranslationServiceManager::translate(const QString &serviceName, const QStringList &sourceTexts, const QVariantMap &settings)
{
    for (const QString &sourceText : sourceTexts) {
        qtlingo::ITranslationService* service = qtlingo::createTranslationService(serviceName, nullptr).release();
        if (service) {
            connect(service, &qtlingo::ITranslationService::translationFinished, this, &TranslationServiceManager::translationFinished);
            connect(service, &qtlingo::ITranslationService::errorOccurred, this, &TranslationServiceManager::errorOccurred);

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
            m_services.append(service);
        } else {
            emit errorOccurred(QString("Failed to create translation service: %1").arg(serviceName));
        }
    }
}
