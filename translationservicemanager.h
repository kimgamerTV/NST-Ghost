#ifndef TRANSLATIONSERVICEMANAGER_H
#define TRANSLATIONSERVICEMANAGER_H

#include <QObject>
#include <QStringList>
#include <QList>
#include <QPointer> // Add this include
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

private:
    QList<qtlingo::ITranslationService*> m_services;
};

#endif // TRANSLATIONSERVICEMANAGER_H
