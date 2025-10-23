#ifndef QTLINGO_ONNX_DISTILBERT_TRANSLATION_SERVICE_H
#define QTLINGO_ONNX_DISTILBERT_TRANSLATION_SERVICE_H

#include "qtlingo/translationservice.h"
#include <QObject>
#include <QString>

#include <onnxruntime_cxx_api.h> // ONNX Runtime C++ API

namespace qtlingo {

class OnnxDistilBertTranslationService : public ITranslationService {
    Q_OBJECT
public:
    explicit OnnxDistilBertTranslationService(QObject *parent = nullptr);
    ~OnnxDistilBertTranslationService() override; // Destructor to clean up ONNX Runtime resources
    QString serviceName() const override { return "ONNX DistilBERT Translation"; }
    TranslationResult translate(const QString &sourceText) override;

    void setModelPath(const QString &path);
    void setTokenizerPath(const QString &path);
    void setTargetLanguage(const QString &language) override;

private:
    QString m_modelPath;
    QString m_tokenizerPath;
    QString m_targetLanguage;

    // ONNX Runtime members
    Ort::Env m_ortEnv;
    Ort::Session *m_ortSession = nullptr;
    // Ort::MemoryInfo m_memoryInfo; // Might be needed for input/output tensors

    void initializeOnnxRuntime();
    void loadModel();

    // Placeholder for tokenizer and detokenizer
    QList<long long> tokenize(const QString &text);
    QString detokenize(const QList<long long> &tokens);
};

} // namespace qtlingo

#endif // QTLINGO_ONNX_DISTILBERT_TRANSLATION_SERVICE_H
