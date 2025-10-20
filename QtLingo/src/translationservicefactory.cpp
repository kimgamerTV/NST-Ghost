#include "qtlingo/translationservicefactory.h"
#include "google_translate_service.h"
#include "llm_translation_service.h"

namespace qtlingo {

std::unique_ptr<ITranslationService> createTranslationService(const QString &serviceName, QObject *parent)
{
    if (serviceName.compare("Google Translate", Qt::CaseInsensitive) == 0) {
        return std::make_unique<GoogleTranslateService>(parent);
    }
    if (serviceName.compare("LLM Translation", Qt::CaseInsensitive) == 0) {
        return std::make_unique<LLMTranslationService>(parent);
    }
    return nullptr;
}

QStringList availableTranslationServices()
{
    return {"Google Translate", "LLM Translation"};
}

} // namespace qtlingo
