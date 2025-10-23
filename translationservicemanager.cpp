#include "translationservicemanager.h"
#include <QDebug>
#include <QThread>
#include <QEventLoop>

#include <QVariantMap>

TranslationServiceManager::TranslationServiceManager(QObject *parent)
    : QObject(parent)
{
}

QStringList TranslationServiceManager::getAvailableServices() const
{
    return qtlingo::availableTranslationServices();
}

void TranslationServiceManager::translate(const QString &serviceName, const QStringList &sourceTexts, const QVariantMap &settings)
{
    for (const QString &sourceText : sourceTexts) {
        QThread* thread = QThread::create([this, serviceName, sourceText, settings]() {
            QEventLoop loop;
            std::unique_ptr<qtlingo::ITranslationService> service = qtlingo::createTranslationService(serviceName, nullptr);
            if (service) {
                connect(service.get(), &qtlingo::ITranslationService::translationFinished, this, &TranslationServiceManager::translationFinished);
                connect(service.get(), &qtlingo::ITranslationService::errorOccurred, this, &TranslationServiceManager::errorOccurred);
                connect(service.get(), &qtlingo::ITranslationService::translationFinished, &loop, &QEventLoop::quit);
                connect(service.get(), &qtlingo::ITranslationService::errorOccurred, &loop, &QEventLoop::quit);

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
                loop.exec();
            } else {
                emit errorOccurred(QString("Failed to create translation service: %1").arg(serviceName));
            }
        });
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        thread->start();
    }
}
