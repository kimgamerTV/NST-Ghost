#ifndef TRANSLATIONSERVICEMANAGER_H
#define TRANSLATIONSERVICEMANAGER_H

#include <QObject>
#include <QStringList>
#include <QList>
#include <QPointer>
#include <QTimer>
#include <QQueue>
#include <qtlingo/translationservice.h>
#include <qtlingo/translationservicefactory.h>
#include <QVariantMap>

class TranslationServiceManager : public QObject
{
    Q_OBJECT
public:
    explicit TranslationServiceManager(QObject *parent = nullptr);
    ~TranslationServiceManager();

    QStringList getAvailableServices() const;
    void translate(const QString &serviceName, const QStringList &sourceTexts, const QVariantMap &settings);

signals:
    void translationFinished(const qtlingo::TranslationResult &result);
    void errorOccurred(const QString &message);
    void progressUpdated(int current, int total);

private slots:
    void processNextTranslation();

private:
    QList<qtlingo::ITranslationService*> m_services;
    QQueue<QString> m_translationQueue;
    QTimer m_timer;
    qtlingo::ITranslationService* m_currentService = nullptr;
    int m_totalItems = 0;
    int m_processedItems = 0;
};

#endif // TRANSLATIONSERVICEMANAGER_H
