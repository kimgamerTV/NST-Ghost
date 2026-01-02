#include "imagetranslationwidget.h"
#include "translationservicemanager.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QClipboard>
#include <QApplication>
#include <QPen>
#include <QPainter>
#include <QFontMetrics>

#if defined(slots)
#undef slots
#endif
#include <pybind11/embed.h>
#include <pybind11/stl.h>
namespace py = pybind11;

struct ImageTranslationWidget::Private {
    py::object translator;
};

ImageTranslationWidget::ImageTranslationWidget(TranslationServiceManager *translationManager, QWidget *parent)
    : QWidget(parent)
    , m_translationManager(translationManager)
    , d(new Private)
{
    setupUi();
    
    // Connect to translation manager signals
    if (m_translationManager) {
        connect(m_translationManager, &TranslationServiceManager::translationFinished,
                this, &ImageTranslationWidget::onTranslationFinished);
        connect(m_translationManager, &TranslationServiceManager::errorOccurred,
                this, &ImageTranslationWidget::onTranslationError);
    }
    
    // Initialize Python
    try {
        py::module_ sys = py::module_::import("sys");
        sys.attr("path").attr("append")(".");
        sys.attr("path").attr("append")("scripts");
        
        py::module_ mod = py::module_::import("scripts.image_translator");
        if (mod.is_none()) mod = py::module_::import("image_translator");
        
        d->translator = mod.attr("ImageTranslator")();
        
        bool available = d->translator.attr("is_available")().cast<bool>();
        if (!available) {
            m_statusLabel->setText("Status: EasyOCR not found. Running in Mock Mode.");
            m_statusLabel->setStyleSheet("color: orange;");
        } else {
            m_statusLabel->setText("Status: Ready (EasyOCR Active)");
            m_statusLabel->setStyleSheet("color: green;");
        }
    } catch (const std::exception &e) {
        qDebug() << "Failed to init ImageTranslator:" << e.what();
        m_statusLabel->setText(QString("Status: Error init Python: %1").arg(e.what()));
        m_statusLabel->setStyleSheet("color: red;");
        d->translator = py::none();
    }
}

ImageTranslationWidget::~ImageTranslationWidget()
{
    delete d;
}

void ImageTranslationWidget::setSettings(const QString &apiKey, const QString &targetLanguage, bool googleApi,
                                          const QString &llmProvider, const QString &llmApiKey,
                                          const QString &llmModel, const QString &llmBaseUrl)
{
    m_apiKey = apiKey;
    m_targetLanguage = targetLanguage;
    m_googleApi = googleApi;
    m_llmProvider = llmProvider;
    m_llmApiKey = llmApiKey;
    m_llmModel = llmModel;
    m_llmBaseUrl = llmBaseUrl;
}

void ImageTranslationWidget::setupUi()
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    
    // Left Side: Image Viewer
    QVBoxLayout *leftLayout = new QVBoxLayout();
    m_imageScene = new QGraphicsScene(this);
    m_imageView = new QGraphicsView(m_imageScene);
    m_imageView->setRenderHint(QPainter::Antialiasing);
    m_imageView->setDragMode(QGraphicsView::ScrollHandDrag);
    leftLayout->addWidget(m_imageView);
    
    // Right Side: Controls
    QWidget *rightContainer = new QWidget(this);
    rightContainer->setFixedWidth(250);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightContainer);
    
    m_btnLoad = new QPushButton("Load Image", this);
    connect(m_btnLoad, &QPushButton::clicked, this, &ImageTranslationWidget::onLoadImage);
    
    m_comboSourceLang = new QComboBox(this);
    m_comboSourceLang->addItem("English", "en");
    m_comboSourceLang->addItem("Japanese", "ja");
    m_comboSourceLang->addItem("Chinese", "ch_sim");
    m_comboSourceLang->addItem("Korean", "ko");
    
    m_editTargetLang = new QLineEdit("th", this);
    m_editTargetLang->setPlaceholderText("Target Lang Code");
    
    m_btnDetect = new QPushButton("Detect Text (OCR)", this);
    connect(m_btnDetect, &QPushButton::clicked, this, &ImageTranslationWidget::onDetectText);
    
    m_btnTranslate = new QPushButton("Translate", this);
    connect(m_btnTranslate, &QPushButton::clicked, this, &ImageTranslationWidget::onTranslate);
    
    m_chkOverlayMode = new QCheckBox("Overlay Mode (Google Lens)", this);
    m_chkOverlayMode->setChecked(false);
    connect(m_chkOverlayMode, &QCheckBox::toggled, this, &ImageTranslationWidget::onOverlayModeChanged);
    
    m_btnCopy = new QPushButton("Copy All Text", this);
    connect(m_btnCopy, &QPushButton::clicked, this, &ImageTranslationWidget::onCopyText);
    
    m_statusLabel = new QLabel("Status: Idle", this);
    m_statusLabel->setWordWrap(true);
    
    rightLayout->addWidget(new QLabel("Source Language:"));
    rightLayout->addWidget(m_comboSourceLang);
    rightLayout->addWidget(new QLabel("Target Language:"));
    rightLayout->addWidget(m_editTargetLang);
    rightLayout->addSpacing(10);
    rightLayout->addWidget(m_btnLoad);
    rightLayout->addWidget(m_btnDetect);
    rightLayout->addWidget(m_btnTranslate);
    rightLayout->addSpacing(10);
    rightLayout->addWidget(m_chkOverlayMode);
    rightLayout->addWidget(m_btnCopy);
    rightLayout->addStretch();
    rightLayout->addWidget(m_statusLabel);
    
    mainLayout->addLayout(leftLayout, 1);
    mainLayout->addWidget(rightContainer, 0);
}

void ImageTranslationWidget::onLoadImage()
{
    QString path = QFileDialog::getOpenFileName(this, "Open Image", "", "Images (*.png *.jpg *.jpeg *.bmp)");
    if (!path.isEmpty()) {
        displayImage(path);
        m_hasDetected = false;
        m_detections = QJsonArray();
        m_translatedTexts.clear();
    }
}

void ImageTranslationWidget::displayImage(const QString &path)
{
    m_currentImagePath = path;
    m_imageScene->clear();
    
    QPixmap pixmap(path);
    if (!pixmap.isNull()) {
        m_imageScene->addPixmap(pixmap);
        m_imageView->setSceneRect(pixmap.rect());
        m_imageView->fitInView(m_imageScene->itemsBoundingRect(), Qt::KeepAspectRatio);
        m_statusLabel->setText("Image Loaded: " + QFileInfo(path).fileName());
    } else {
        QMessageBox::warning(this, "Error", "Failed to load image.");
    }
}

void ImageTranslationWidget::onDetectText()
{
    if (m_currentImagePath.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please load an image first.");
        return;
    }
    
    if (d->translator.is_none()) {
        QMessageBox::critical(this, "Error", "OCR not initialized.");
        return;
    }
    
    m_statusLabel->setText("Detecting text...");
    QApplication::processEvents();
    
    QString sourceLang = m_comboSourceLang->currentData().toString();
    
    try {
        std::string resInfo = d->translator.attr("translate_image")(
            m_currentImagePath.toStdString(),
            sourceLang.toStdString(),
            "th" // dummy
        ).cast<std::string>();
        
        QString jsonStr = QString::fromStdString(resInfo);
        QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
        
        if (doc.isObject() && doc.object().contains("error")) {
            m_statusLabel->setText("Error: " + doc.object()["error"].toString());
            return;
        }
        
        if (doc.isArray()) {
            m_detections = doc.array();
            m_hasDetected = true;
            m_translatedTexts.clear();
            drawDetections(false);
            m_statusLabel->setText(QString("Detected %1 text regions.").arg(m_detections.size()));
        }
        
    } catch (const std::exception &e) {
        qDebug() << "OCR Error:" << e.what();
        m_statusLabel->setText("OCR Error.");
    }
}

void ImageTranslationWidget::onTranslate()
{
    if (!m_hasDetected) {
        onDetectText();
        if (!m_hasDetected) return;
    }
    
    if (m_detections.isEmpty()) {
        QMessageBox::information(this, "Info", "No text detected to translate.");
        return;
    }
    
    if (!m_translationManager) {
        QMessageBox::critical(this, "Error", "Translation service not available.");
        return;
    }
    
    m_statusLabel->setText("Translating...");
    m_translatedTexts.clear();
    m_translatedTexts.reserve(m_detections.size());
    
    // Collect all texts
    QStringList textsToTranslate;
    for (const QJsonValue &val : m_detections) {
        textsToTranslate.append(val.toObject()["text"].toString());
    }
    
    // Build settings
    QVariantMap settings;
    settings["targetLanguage"] = m_editTargetLang->text();
    settings["googleApi"] = m_googleApi;
    settings["googleApiKey"] = m_apiKey;
    settings["llmProvider"] = m_llmProvider;
    settings["llmApiKey"] = m_llmApiKey;
    settings["llmModel"] = m_llmModel;
    
    // Use the service configured in settings
    // Default to Google Translate, use LLM only if properly configured
    QString serviceName = "Google Translate";
    if (!m_llmProvider.isEmpty() && !m_llmApiKey.isEmpty() && !m_llmModel.isEmpty()) {
        serviceName = "LLM Translation";
    }
    
    m_currentTranslationIndex = 0;
    m_translationManager->translate(serviceName, textsToTranslate, settings);
}

void ImageTranslationWidget::onTranslationFinished(const qtlingo::TranslationResult &result)
{
    m_translatedTexts.append(result.translatedText);
    m_currentTranslationIndex++;
    
    m_statusLabel->setText(QString("Translating... %1/%2").arg(m_currentTranslationIndex).arg(m_detections.size()));
    
    if (m_currentTranslationIndex >= m_detections.size()) {
        if (m_overlayMode) {
            drawOverlayMode();
        } else {
            drawDetections(true);
        }
        m_statusLabel->setText("Translation Complete.");
    }
}

void ImageTranslationWidget::onTranslationError(const QString &message)
{
    m_statusLabel->setText("Translation Error: " + message);
}

void ImageTranslationWidget::onOverlayModeChanged(bool checked)
{
    m_overlayMode = checked;
    
    // Redraw if we have translations
    if (!m_translatedTexts.isEmpty()) {
        if (m_overlayMode) {
            drawOverlayMode();
        } else {
            drawDetections(true);
        }
    }
}

void ImageTranslationWidget::drawDetections(bool showTranslated)
{
    m_imageScene->clear();
    QPixmap pixmap(m_currentImagePath);
    m_imageScene->addPixmap(pixmap);
    
    QPen pen(Qt::red);
    pen.setWidth(3);
    
    QFont font("Arial", 12, QFont::Bold);
    
    for (int i = 0; i < m_detections.size(); ++i) {
        QJsonObject obj = m_detections[i].toObject();
        QString text = obj["text"].toString();
        
        if (showTranslated && i < m_translatedTexts.size()) {
            text = m_translatedTexts[i];
        }
        
        QJsonArray bbox = obj["bbox"].toArray();
        
        if (bbox.size() >= 4) {
            QJsonArray p1 = bbox[0].toArray();
            QJsonArray p3 = bbox[2].toArray();
            
            int x = p1[0].toInt();
            int y = p1[1].toInt();
            int w = p3[0].toInt() - x;
            int h = p3[1].toInt() - y;
            
            m_imageScene->addRect(x, y, w, h, pen);
            
            QGraphicsTextItem *textItem = m_imageScene->addText(text, font);
            textItem->setDefaultTextColor(Qt::white);
            textItem->setPos(x, y - 25);
            
            QRectF textRect = textItem->boundingRect();
            QGraphicsRectItem *bgItem = m_imageScene->addRect(x, y - 25, textRect.width(), textRect.height(), Qt::NoPen, QBrush(Qt::black));
            bgItem->setOpacity(0.7);
            textItem->setZValue(1);
        }
    }
}

void ImageTranslationWidget::drawOverlayMode()
{
    // Google Lens style: overlay translated text on top of original
    QPixmap originalPixmap(m_currentImagePath);
    QPixmap resultPixmap = originalPixmap.copy();
    
    QPainter painter(&resultPixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);
    
    for (int i = 0; i < m_detections.size(); ++i) {
        if (i >= m_translatedTexts.size()) break;
        
        QJsonObject obj = m_detections[i].toObject();
        QString translatedText = m_translatedTexts[i];
        QJsonArray bbox = obj["bbox"].toArray();
        
        if (bbox.size() >= 4) {
            QJsonArray p1 = bbox[0].toArray();
            QJsonArray p3 = bbox[2].toArray();
            
            int x = p1[0].toInt();
            int y = p1[1].toInt();
            int w = p3[0].toInt() - x;
            int h = p3[1].toInt() - y;
            
            QRect textRect(x, y, w, h);
            
            // Fill background to cover original text
            painter.fillRect(textRect, QColor(255, 255, 255, 230));
            
            // Calculate font size to fit in box
            int fontSize = h * 0.7;
            if (fontSize < 8) fontSize = 8;
            if (fontSize > 72) fontSize = 72;
            
            QFont font("Arial", fontSize, QFont::Bold);
            painter.setFont(font);
            
            // Adjust font size if text doesn't fit
            QFontMetrics fm(font);
            while (fm.horizontalAdvance(translatedText) > w && fontSize > 8) {
                fontSize--;
                font.setPointSize(fontSize);
                painter.setFont(font);
                fm = QFontMetrics(font);
            }
            
            // Draw text centered in box
            painter.setPen(Qt::black);
            painter.drawText(textRect, Qt::AlignCenter | Qt::TextWordWrap, translatedText);
        }
    }
    
    painter.end();
    
    // Display result
    m_imageScene->clear();
    m_imageScene->addPixmap(resultPixmap);
}

void ImageTranslationWidget::onCopyText()
{
    if (!m_hasDetected || m_detections.isEmpty()) {
        QMessageBox::information(this, "Info", "No text detected. Please detect text first.");
        return;
    }
    
    QStringList allTexts;
    
    if (!m_translatedTexts.isEmpty()) {
        allTexts = m_translatedTexts;
    } else {
        for (const QJsonValue &val : m_detections) {
            allTexts.append(val.toObject()["text"].toString());
        }
    }
    
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(allTexts.join("\n"));
    
    m_statusLabel->setText("Text copied to clipboard.");
}
