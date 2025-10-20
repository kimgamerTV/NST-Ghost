#ifndef TRANSLATIONSERVICEMANAGER_H
#define TRANSLATIONSERVICEMANAGER_H

#include <QObject>
#include <QStringList>
#include <QMap>
#include <memory>

#include <qtlingo/translationservice.h>
#include <qtlingo/translationservicefactory.h>

#include <QVariantMap>

class TranslationServiceManager : public QObject
{
    Q_OBJECT
public:
    explicit TranslationServiceManager(QObject *parent = nullptr);

    QStringList getAvailableServices() const;
    void translate(const QString &serviceName, const QString &sourceText, const QVariantMap &settings);

signals:
    void translationFinished(const qtlingo::TranslationResult &result);
    void errorOccurred(const QString &message);

private slots:
    void handleTranslationFinished(const qtlingo::TranslationResult &result);
    void handleErrorOccurred(const QString &message);

private:
    std::unique_ptr<qtlingo::ITranslationService> m_currentService;
};

#endif // TRANSLATIONSERVICEMANAGER_H
