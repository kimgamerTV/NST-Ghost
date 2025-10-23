#include "onnx_distilbert_translation_service.h"
#include <QDebug>
#include <QFile>

namespace qtlingo {

OnnxDistilBertTranslationService::OnnxDistilBertTranslationService(QObject *parent)
    : ITranslationService(parent),
      m_ortEnv(ORT_LOGGING_LEVEL_WARNING, "OnnxDistilBertTranslator")
{
    qDebug() << "ONNX DistilBERT Translation Service created.";
    initializeOnnxRuntime();
}

OnnxDistilBertTranslationService::~OnnxDistilBertTranslationService()
{
    if (m_ortSession) {
        delete m_ortSession;
        m_ortSession = nullptr;
    }
    qDebug() << "ONNX DistilBERT Translation Service destroyed.";
}

void OnnxDistilBertTranslationService::initializeOnnxRuntime()
{
    // Ort::SessionOptions sessionOptions;
    // sessionOptions.SetIntraOpNumThreads(1);
    // sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);
    qDebug() << "ONNX Runtime initialized.";
}

void OnnxDistilBertTranslationService::loadModel()
{
    if (m_modelPath.isEmpty()) {
        qWarning() << "ONNX model path is empty. Cannot load model.";
        return;
    }

    if (!QFile::exists(m_modelPath)) {
        qWarning() << "ONNX model file does not exist:" << m_modelPath;
        return;
    }

    if (m_ortSession) {
        delete m_ortSession;
        m_ortSession = nullptr;
    }

    try {
        m_ortSession = new Ort::Session(m_ortEnv, m_modelPath.toStdString().c_str(), Ort::SessionOptions{nullptr});
        qDebug() << "ONNX model loaded from:" << m_modelPath;
    } catch (const Ort::Exception& e) {
        qCritical() << "Error loading ONNX model:" << e.what();
        emit errorOccurred(QString("Error loading ONNX model: %1").arg(e.what()));
    }
}

TranslationResult OnnxDistilBertTranslationService::translate(const QString &sourceText)
{
    qDebug() << "Analyzing with ONNX DistilBERT:" << sourceText;

    if (!m_ortSession) {
        qWarning() << "ONNX model not loaded. Cannot perform analysis.";
        TranslationResult result;
        result.sourceText = sourceText;
        result.translatedText = "";
        emit errorOccurred("ONNX model not loaded.");
        return result;
    }

    // 1. Tokenize the source text
    QList<long long> inputTokens = tokenize(sourceText);
    qDebug() << "Input tokens:" << inputTokens;

    // 2. Placeholder for ONNX Runtime inference
    // In a real scenario, you would create input tensors from inputTokens,
    // run m_ortSession->Run(), and get output tensors.
    // For now, let's simulate some output tokens.
    QList<long long> outputTokens = inputTokens; // Dummy: output same as input

    // 3. Detokenize the output tokens to get a suggested translation
    QString suggestedTranslation = detokenize(outputTokens);

    TranslationResult result;
    result.sourceText = sourceText;
    result.translatedText = ""; // No direct translation from this helper service

    // Dummy suggestion for demonstration
    TranslationSuggestion suggestion;
    suggestion.originalSegment = sourceText;
    suggestion.suggestedTranslation = suggestedTranslation.toUpper(); // Using detokenized output
    result.suggestions.append(suggestion);

    emit translationFinished(result);
    return result;
}

void OnnxDistilBertTranslationService::setModelPath(const QString &path)
{
    if (m_modelPath != path) {
        m_modelPath = path;
        qDebug() << "ONNX DistilBERT Model Path set to:" << m_modelPath;
        loadModel(); // Attempt to load model when path changes
    }
}

void OnnxDistilBertTranslationService::setTokenizerPath(const QString &path)
{
    m_tokenizerPath = path;
    qDebug() << "ONNX DistilBERT Tokenizer Path set to:" << m_tokenizerPath;
}

void OnnxDistilBertTranslationService::setTargetLanguage(const QString &language)
{
    m_targetLanguage = language;
    qDebug() << "ONNX DistilBERT Target Language set to:" << m_targetLanguage;
}

QList<long long> OnnxDistilBertTranslationService::tokenize(const QString &text)
{
    qDebug() << "[TOKENIZER]: Tokenizing text:" << text;
    // Placeholder: In a real scenario, this would use a WordPiece tokenizer
    // For now, let's return some dummy token IDs
    QList<long long> dummyTokens;
    for (QChar c : text) {
        dummyTokens.append(c.unicode()); // Simple char to int conversion
    }
    return dummyTokens;
}

QString OnnxDistilBertTranslationService::detokenize(const QList<long long> &tokens)
{
    qDebug() << "[DETOKENIZER]: Detokenizing tokens.";
    // Placeholder: In a real scenario, this would convert token IDs back to text
    QString detokenizedText;
    for (long long token : tokens) {
        detokenizedText.append(QChar(token)); // Simple int to char conversion
    }
    return detokenizedText;
}

} // namespace qtlingo
