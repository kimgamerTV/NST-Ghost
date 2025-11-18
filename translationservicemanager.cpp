#include "translationservicemanager.h"
#include <QDebug>

TranslationServiceManager::TranslationServiceManager(QObject *parent)
    : QObject(parent)
{
    connect(&m_timer, &QTimer::timeout, this, &TranslationServiceManager::processNextTranslation);
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
    if (sourceTexts.isEmpty()) return;

    m_currentService = qtlingo::createTranslationService(serviceName, nullptr).release();
    if (!m_currentService) {
        emit errorOccurred(QString("Failed to create translation service: %1").arg(serviceName));
        return;
    }

    connect(m_currentService, &qtlingo::ITranslationService::translationFinished, this, &TranslationServiceManager::translationFinished);
    connect(m_currentService, &qtlingo::ITranslationService::errorOccurred, this, &TranslationServiceManager::errorOccurred);

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
        m_currentService->setTargetLanguage(settings.value("targetLanguage").toString());
    }

    m_translationQueue.clear();
    for (const QString &text : sourceTexts) {
        m_translationQueue.enqueue(text);
    }
    
    m_totalItems = sourceTexts.size();
    m_processedItems = 0;
    m_services.append(m_currentService);
    
    processNextTranslation();
}

void TranslationServiceManager::processNextTranslation()
{
    if (m_translationQueue.isEmpty()) {
        m_timer.stop();
        emit progressUpdated(m_totalItems, m_totalItems);
        return;
    }

    QString text = m_translationQueue.dequeue();
    m_currentService->translate(text);
    m_processedItems++;
    
    emit progressUpdated(m_processedItems, m_totalItems);
    
    if (!m_translationQueue.isEmpty() && !m_timer.isActive()) {
        m_timer.start(500);
    }
}
