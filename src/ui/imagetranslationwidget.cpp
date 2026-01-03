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
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // --- TOP TOOLBAR ---
    QWidget *topToolbar = new QWidget(this);
    topToolbar->setFixedHeight(60);
    topToolbar->setStyleSheet("background-color: #2D2D2D; border-bottom: 1px solid #3E3E3E;");
    QHBoxLayout *toolbarLayout = new QHBoxLayout(topToolbar);
    toolbarLayout->setContentsMargins(15, 10, 15, 10);
    
    m_btnLoad = new QPushButton("Open Image", this);
    m_btnLoad->setIcon(QIcon::fromTheme("document-open"));
    m_btnLoad->setStyleSheet("QPushButton { padding: 6px 12px; background-color: #444; border-radius: 4px; color: white; } QPushButton:hover { background-color: #555; }");
    connect(m_btnLoad, &QPushButton::clicked, this, &ImageTranslationWidget::onLoadImage);
    
    m_btnSave = new QPushButton("Save Result", this);
    m_btnSave->setEnabled(false);
    m_btnSave->setStyleSheet("QPushButton { padding: 6px 12px; background-color: #444; border-radius: 4px; color: white; } QPushButton:hover { background-color: #555; }");
    connect(m_btnSave, &QPushButton::clicked, this, &ImageTranslationWidget::onSaveImage);
    
    m_comboSourceLang = new QComboBox(this);
    m_comboSourceLang->addItem("English", "en");
    m_comboSourceLang->addItem("Japanese", "ja");
    m_comboSourceLang->addItem("Chinese", "ch_sim");
    m_comboSourceLang->addItem("Korean", "ko");
    m_comboSourceLang->setStyleSheet("padding: 5px;");
    
    QLabel *arrowLabel = new QLabel("->", this);
    arrowLabel->setStyleSheet("color: white; font-weight: bold;");
    
    m_editTargetLang = new QLineEdit("th", this);
    m_editTargetLang->setFixedWidth(50);
    m_editTargetLang->setAlignment(Qt::AlignCenter);
    m_editTargetLang->setStyleSheet("padding: 5px;");
    
    m_btnTranslate = new QPushButton("RUN TRANSLATION", this);
    m_btnTranslate->setIcon(QIcon::fromTheme("system-run"));
    m_btnTranslate->setStyleSheet("QPushButton { padding: 8px 16px; background-color: #007ACC; border-radius: 4px; color: white; font-weight: bold; } QPushButton:hover { background-color: #0098FF; }");
    connect(m_btnTranslate, &QPushButton::clicked, this, &ImageTranslationWidget::onTranslate);
    
    toolbarLayout->addWidget(m_btnLoad);
    toolbarLayout->addWidget(m_btnSave);
    toolbarLayout->addStretch();
    toolbarLayout->addWidget(m_comboSourceLang);
    toolbarLayout->addWidget(arrowLabel);
    toolbarLayout->addWidget(m_editTargetLang);
    toolbarLayout->addSpacing(20);
    toolbarLayout->addWidget(m_btnTranslate);
    
    mainLayout->addWidget(topToolbar);
    
    // --- MAIN CONTENT (Image View) ---
    m_imageScene = new QGraphicsScene(this);
    m_imageView = new QGraphicsView(m_imageScene);
    m_imageView->setRenderHint(QPainter::Antialiasing);
    m_imageView->setDragMode(QGraphicsView::ScrollHandDrag);
    m_imageView->setStyleSheet("background-color: #1E1E1E; border: none;");
    mainLayout->addWidget(m_imageView, 1);
    
    // --- BOTTOM CONTROL BAR (Floating Style) ---
    QWidget *bottomBar = new QWidget(this);
    bottomBar->setFixedHeight(50);
    bottomBar->setStyleSheet("background-color: #252526; border-top: 1px solid #3E3E3E;");
    QHBoxLayout *bottomLayout = new QHBoxLayout(bottomBar);
    bottomLayout->setContentsMargins(20, 5, 20, 5);
    
    m_statusLabel = new QLabel("Ready.", this);
    m_statusLabel->setStyleSheet("color: #AAAAAA;");
    
    // View Switcher (Segmented Control style)
    QWidget *viewSwitcher = new QWidget(this);
    QHBoxLayout *switcherLayout = new QHBoxLayout(viewSwitcher);
    switcherLayout->setContentsMargins(0,0,0,0);
    switcherLayout->setSpacing(0);
    
    m_viewGroup = new QButtonGroup(this);
    m_viewGroup->setExclusive(true);
    
    // Helper to style segmented buttons
    QString baseStyle = "QPushButton { padding: 6px 15px; background-color: #333; color: #CCC; border: 1px solid #444; } QPushButton:checked { background-color: #555; color: white; border: 1px solid #666; }";
    QString leftStyle = baseStyle + "QPushButton { border-top-left-radius: 4px; border-bottom-left-radius: 4px; }";
    QString midStyle = baseStyle + "QPushButton { border-radius: 0px; border-left: none; }";
    QString rightStyle = baseStyle + "QPushButton { border-top-right-radius: 4px; border-bottom-right-radius: 4px; border-left: none; }";
    
    m_btnViewOriginal = new QPushButton("Original", this);
    m_btnViewOriginal->setCheckable(true);
    m_btnViewOriginal->setChecked(true);
    m_btnViewOriginal->setStyleSheet(leftStyle);
    m_viewGroup->addButton(m_btnViewOriginal, Original);
    
    m_btnViewClean = new QPushButton("Clean (Inpainted)", this);
    m_btnViewClean->setCheckable(true);
    m_btnViewClean->setStyleSheet(midStyle);
    m_viewGroup->addButton(m_btnViewClean, Clean);
    
    m_btnViewTranslated = new QPushButton("Translated", this);
    m_btnViewTranslated->setCheckable(true);
    m_btnViewTranslated->setStyleSheet(rightStyle);
    m_viewGroup->addButton(m_btnViewTranslated, Translated);
    
    connect(m_viewGroup, SIGNAL(idClicked(int)), this, SLOT(onViewModeChanged(int)));
    
    switcherLayout->addWidget(m_btnViewOriginal);
    switcherLayout->addWidget(m_btnViewClean);
    switcherLayout->addWidget(m_btnViewTranslated);
    
    m_btnPeekOriginal = new QPushButton("Peek Original", this);
    m_btnPeekOriginal->setStyleSheet("QPushButton { margin-left: 20px; padding: 6px 12px; background-color: #444; border-radius: 4px; color: white; } QPushButton:pressed { background-color: #666; }");
    connect(m_btnPeekOriginal, &QPushButton::pressed, this, &ImageTranslationWidget::onPeekPressed);
    connect(m_btnPeekOriginal, &QPushButton::released, this, &ImageTranslationWidget::onPeekReleased);
    
    bottomLayout->addWidget(m_statusLabel);
    bottomLayout->addStretch();
    bottomLayout->addWidget(viewSwitcher);
    bottomLayout->addWidget(m_btnPeekOriginal);
    bottomLayout->addStretch(); // Center the switcher
    
    mainLayout->addWidget(bottomBar);
}

void ImageTranslationWidget::onLoadImage()
{
    QString path = QFileDialog::getOpenFileName(this, "Open Image", "", "Images (*.png *.jpg *.jpeg *.bmp)");
    if (!path.isEmpty()) {
        displayImage(path);
        m_hasDetected = false;
        m_detections = QJsonArray();
        m_translatedTexts.clear();
        m_inpaintedImagePath.clear();
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

void ImageTranslationWidget::onTranslate()
{
    if (m_currentImagePath.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please load an image first.");
        return;
    }
    
    if (d->translator.is_none()) {
        QMessageBox::critical(this, "Error", "OCR/AI backend not initialized.");
        return;
    }

    m_statusLabel->setText("Status: Analyzing Image (AI)...");
    m_btnTranslate->setEnabled(false);
    m_btnSave->setEnabled(false);
    QApplication::processEvents();

    // Step 1: Detect Text & Inpaint (Python Side)
    // We execute this in the main thread for now to keep pybind11 simple, 
    // though ideally it should be threaded.
    
    try {
        // A. Translate Image Method does detection + basic translation, but we need raw detection first
        // actually let's use the 'translate_image' for detection roughly first or 'readText' if available?
        // Revisiting python script: 'translate_image' does detection.
        // But 'inpaint_text_regions' needs bounding boxes.
        
        // Let's call Python: translate_image (for OCR) -> then inpaint_text_regions
        
        QString sourceLang = m_comboSourceLang->currentData().toString();
        
        m_statusLabel->setText("Status: 1/3 Performing OCR & Detection...");
        QApplication::processEvents();
        
        std::string resInfo = d->translator.attr("translate_image")(
            m_currentImagePath.toStdString(),
            sourceLang.toStdString(),
            "th" // dummy target
        ).cast<std::string>();
        
        QJsonDocument doc = QJsonDocument::fromJson(QString::fromStdString(resInfo).toUtf8());
        if (!doc.isArray()) {
            m_statusLabel->setText("Status: Error (No text found or invalid response)");
            m_btnTranslate->setEnabled(true);
            return;
        }
        m_detections = doc.array();
        
        if (m_detections.isEmpty()) {
             m_statusLabel->setText("Status: No text detected.");
             m_btnTranslate->setEnabled(true);
             return;
        }

        // Step 2: Advanced Analysis & Inpainting (LaMa)
        m_statusLabel->setText("Status: 2/3 AI Inpainting (LaMa) & Color Extraction...");
        QApplication::processEvents();
        
        QString detectionsJson = QString::fromUtf8(QJsonDocument(m_detections).toJson(QJsonDocument::Compact));
        std::string enrichedJsonStr = d->translator.attr("inpaint_text_regions")(
            m_currentImagePath.toStdString(),
            detectionsJson.toStdString()
        ).cast<std::string>();
        
        QJsonDocument enrichedDoc = QJsonDocument::fromJson(QString::fromStdString(enrichedJsonStr).toUtf8());
        if (enrichedDoc.isObject()) {
             QJsonObject root = enrichedDoc.object();
             m_inpaintedImagePath = root["inpainted_path"].toString();
             if (root.contains("detections")) {
                 m_detections = root["detections"].toArray(); // Enriched with angles/colors
             }
        }
        
    } catch (const std::exception &e) {
        QString errorMsg = QString::fromUtf8(e.what());
        qDebug() << "AI Backend Error:" << errorMsg;
        m_statusLabel->setText("Error: " + errorMsg);
        m_btnTranslate->setEnabled(true);
        return;
    }
    
    // Step 3: Run Text Translation
    m_statusLabel->setText("Status: 3/3 Translating Text...");
    m_translatedTexts.clear();
    m_translatedTexts.reserve(m_detections.size());
    
    QStringList textsToTranslate;
    for (const QJsonValue &val : m_detections) {
        textsToTranslate.append(val.toObject()["text"].toString());
    }
    
    // Prepare Settings
    QVariantMap settings;
    settings["targetLanguage"] = m_editTargetLang->text();
    settings["googleApi"] = m_googleApi;
    settings["googleApiKey"] = m_apiKey;
    
    // Use Translation Service
    QString serviceName = "Google Translate"; // Default
    // Note: LLM settings handled by TranslationServiceManager internal logic
    
    m_currentTranslationIndex = 0;
    // Trigger translation
    m_translationManager->translate(serviceName, textsToTranslate, settings);
}

void ImageTranslationWidget::onTranslationFinished(const qtlingo::TranslationResult &result)
{
    m_translatedTexts.append(result.translatedText);
    m_currentTranslationIndex++;
    
    int pct = (m_currentTranslationIndex * 100) / m_detections.size();
    m_statusLabel->setText(QString("Status: Translating... %1%").arg(pct));
    
    if (m_currentTranslationIndex >= m_detections.size()) {
        m_statusLabel->setText("Status: Finished.");
        m_btnTranslate->setEnabled(true);
        m_btnSave->setEnabled(true);
        
        // Auto-switch to Translated View on finish
        m_currentViewMode = Translated;
        m_viewGroup->button(Translated)->setChecked(true);
        updateViewMode();
    }
}

void ImageTranslationWidget::onTranslationError(const QString &message)
{
     m_statusLabel->setText("Translation Error: " + message);
     m_btnTranslate->setEnabled(true);
}

// Removed legacy: onDetectText, onOverlayModeChanged, drawDetections, drawOverlayMode

void ImageTranslationWidget::onSaveImage()
{
    if (m_currentImagePath.isEmpty()) return;
    
    // Save current content of graphics scene (what the user sees)
    QString savePath = QFileDialog::getSaveFileName(this, "Save Translation", "", "Images (*.png *.jpg)");
    if (!savePath.isEmpty()) {
        QImage image(m_imageScene->sceneRect().size().toSize(), QImage::Format_ARGB32);
        image.fill(Qt::transparent);
        
        QPainter painter(&image);
        m_imageScene->render(&painter);
        
        image.save(savePath);
        m_statusLabel->setText("Saved to: " + savePath);
    }
}

void ImageTranslationWidget::onViewModeChanged(int id)
{
    m_currentViewMode = static_cast<ViewMode>(id);
    updateViewMode();
}

void ImageTranslationWidget::onPeekPressed()
{
    // Temporarily show original
    m_imageScene->clear();
    m_imageScene->addPixmap(QPixmap(m_currentImagePath));
}

void ImageTranslationWidget::onPeekReleased()
{
    // Restore current view
    updateViewMode();
}

void ImageTranslationWidget::updateViewMode()
{
    m_imageScene->clear();
    
    if (m_currentImagePath.isEmpty()) return;
    
    QPixmap displayPixmap(m_currentImagePath); // Default
    
    if (m_currentViewMode == Clean || m_currentViewMode == Translated) {
        if (!m_inpaintedImagePath.isEmpty()) {
            displayPixmap = QPixmap(m_inpaintedImagePath);
        }
    }
    
    // Add Base Image
    m_imageScene->addPixmap(displayPixmap);
    
    // Render Text if in Translated Mode
    if (m_currentViewMode == Translated && !m_translatedTexts.isEmpty()) {
        QPainter pixmapPainter(&displayPixmap);
        pixmapPainter.setRenderHint(QPainter::Antialiasing);
        pixmapPainter.setRenderHint(QPainter::TextAntialiasing);
        
        // --- TEXT RENDERING LOGIC (Moved from drawOverlayMode) ---
        for (int i = 0; i < m_detections.size(); ++i) {
            if (i >= m_translatedTexts.size()) break;
            
            QJsonObject obj = m_detections[i].toObject();
            QString translatedText = m_translatedTexts[i];
            QJsonArray bbox = obj["bbox"].toArray();
            bool isDark = obj["is_dark"].toBool();
            
            if (bbox.size() >= 4) {
                QJsonArray p1 = bbox[0].toArray();
                int x = p1[0].toInt();
                int y = p1[1].toInt();
                
                QJsonArray p3 = bbox[2].toArray();
                int w = p3[0].toInt() - x;
                int h = p3[1].toInt() - y;
                
                QRect textRect(x, y, w, h);
                
                int fontSize = h * 0.75;
                if (fontSize < 8) fontSize = 8;
                if (fontSize > 100) fontSize = 100;
                
                QFont font("Outfit", fontSize, QFont::Bold);
                
                QFontMetrics fm(font);
                while (fm.horizontalAdvance(translatedText) > w && fontSize > 8) {
                    fontSize--;
                    font.setPointSize(fontSize);
                    fm = QFontMetrics(font);
                }
                pixmapPainter.setFont(font);
                
                // Color Extraction Match
                QColor textColor = Qt::black;
                if (obj.contains("text_color")) {
                    QJsonArray rgb = obj["text_color"].toArray();
                    if (rgb.size() == 3) textColor = QColor(rgb[0].toInt(), rgb[1].toInt(), rgb[2].toInt());
                } else {
                    textColor = isDark ? Qt::white : Qt::black;
                }
                
                QColor haloColor = isDark ? QColor(0,0,0,150) : QColor(255,255,255,200);
                double angle = obj["angle"].toDouble();
                
                pixmapPainter.save();
                int centerX = x + w / 2;
                int centerY = y + h / 2;
                pixmapPainter.translate(centerX, centerY);
                pixmapPainter.rotate(angle);
                pixmapPainter.translate(-centerX, -centerY);
                
                QPainterPath path;
                path.addText(x + (w - fm.horizontalAdvance(translatedText))/2, y + (h + fm.ascent() - fm.descent())/2, font, translatedText);
                
                QPen pen(haloColor);
                pen.setWidth(3);
                pixmapPainter.setPen(pen);
                pixmapPainter.setBrush(Qt::NoBrush);
                pixmapPainter.drawPath(path);
                
                pixmapPainter.setPen(Qt::NoPen);
                pixmapPainter.setBrush(textColor);
                pixmapPainter.drawPath(path);
                
                pixmapPainter.restore();
            }
        }
        
        // Re-add to scene since we modified the pixmap
        m_imageScene->clear();
        m_imageScene->addPixmap(displayPixmap);
    }
}
