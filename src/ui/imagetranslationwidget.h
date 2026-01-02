#ifndef IMAGETRANSLATIONWIDGET_H
#define IMAGETRANSLATIONWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QScrollArea>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QLineEdit>
#include <QJsonArray>
#include <QCheckBox>
#include <qtlingo/translationservice.h>

class TranslationServiceManager;

class ImageTranslationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ImageTranslationWidget(TranslationServiceManager *translationManager, QWidget *parent = nullptr);
    ~ImageTranslationWidget();
    
    void setSettings(const QString &apiKey, const QString &targetLanguage, bool googleApi,
                     const QString &llmProvider, const QString &llmApiKey, 
                     const QString &llmModel, const QString &llmBaseUrl);

public slots:
    void onLoadImage();
    void onDetectText();
    void onTranslate();
    void onCopyText();
    void onOverlayModeChanged(bool checked);

private slots:
    void onTranslationFinished(const qtlingo::TranslationResult &result);
    void onTranslationError(const QString &message);

private:
    void setupUi();
    void displayImage(const QString &path);
    void drawDetections(bool showTranslated = false);
    void drawOverlayMode();  // Google Lens style
    
    // UI Elements
    QPushButton *m_btnLoad;
    QPushButton *m_btnDetect;
    QPushButton *m_btnTranslate;
    QPushButton *m_btnCopy;
    QComboBox *m_comboSourceLang;
    QLineEdit *m_editTargetLang;
    QCheckBox *m_chkOverlayMode;
    
    QGraphicsView *m_imageView;
    QGraphicsScene *m_imageScene;
    QLabel *m_statusLabel;
    
    QString m_currentImagePath;
    QJsonArray m_detections;
    QStringList m_translatedTexts;
    bool m_hasDetected = false;
    bool m_overlayMode = false;
    
    // Translation state
    TranslationServiceManager *m_translationManager;
    int m_currentTranslationIndex = 0;
    
    // Settings
    QString m_apiKey;
    QString m_targetLanguage;
    bool m_googleApi = false;
    QString m_llmProvider;
    QString m_llmApiKey;
    QString m_llmModel;
    QString m_llmBaseUrl;
    
    // PIMPL to hide Python dependencies
    struct Private;
    Private *d;
};

#endif // IMAGETRANSLATIONWIDGET_H
