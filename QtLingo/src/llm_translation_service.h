#ifndef QTLINGO_LLM_TRANSLATION_SERVICE_H
#define QTLINGO_LLM_TRANSLATION_SERVICE_H

#include "qtlingo/translationservice.h"

namespace qtlingo {

class LLMTranslationService : public ITranslationService {
    Q_OBJECT
public:
    LLMTranslationService(QObject *parent = nullptr);
    QString serviceName() const override { return "LLM Translation"; }
    TranslationResult translate(const QString &sourceText) override;

    void setApiKey(const QString &apiKey) override;
    void setLlmProvider(const QString &provider) override;
    void setLlmModel(const QString &model) override;

private:
    QString m_apiKey;
    QString m_provider;
    QString m_model;
};

} // namespace qtlingo

#endif // QTLINGO_LLM_TRANSLATION_SERVICE_H
