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
        
        // Get detailed device info
        py::dict deviceInfo = d->translator.attr("get_device_info")().cast<py::dict>();
        bool available = deviceInfo["available"].cast<bool>();
        bool useGpu = deviceInfo["use_gpu"].cast<bool>();
        QString deviceName = QString::fromStdString(deviceInfo["device_name"].cast<std::string>());
        QString gpuStatus = QString::fromStdString(deviceInfo["status"].cast<std::string>());
        
        if (!available) {
            m_statusLabel->setText("Status: EasyOCR not found. Running in Mock Mode.");
            m_statusLabel->setStyleSheet("color: orange;");
        } else if (useGpu) {
            m_statusLabel->setText(QString("Status: Ready (%1)").arg(deviceName));
            m_statusLabel->setStyleSheet("color: #00FF7F;"); // Spring green
        } else {
            // CPU mode - show warning with reason
            m_statusLabel->setText(QString("Status: Ready (CPU Mode) - %1").arg(gpuStatus));
            m_statusLabel->setStyleSheet("color: #FFD700;"); // Gold/yellow warning
        }
        
        qDebug() << "ImageTranslator initialized:" << gpuStatus;
        
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
    
    m_btnLoad = new QPushButton("Add Images", this);
    m_btnLoad->setIcon(QIcon::fromTheme("document-open"));
    m_btnLoad->setStyleSheet("QPushButton { padding: 6px 12px; background-color: #444; border-radius: 4px; color: white; } QPushButton:hover { background-color: #555; }");
    connect(m_btnLoad, &QPushButton::clicked, this, &ImageTranslationWidget::onAddImages);
    
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
    
    m_btnTranslate = new QPushButton("Translate", this);
    m_btnTranslate->setIcon(QIcon::fromTheme("system-run"));
    m_btnTranslate->setStyleSheet("QPushButton { padding: 8px 16px; background-color: #007ACC; border-radius: 4px; color: white; font-weight: bold; } QPushButton:hover { background-color: #0098FF; }");
    connect(m_btnTranslate, &QPushButton::clicked, this, &ImageTranslationWidget::onTranslate);
    
    m_btnTranslateAll = new QPushButton("Translate All", this);
    m_btnTranslateAll->setStyleSheet("QPushButton { padding: 8px 16px; background-color: #28A745; border-radius: 4px; color: white; font-weight: bold; } QPushButton:hover { background-color: #2FCB50; }");
    connect(m_btnTranslateAll, &QPushButton::clicked, this, &ImageTranslationWidget::onTranslateAll);
    
    m_btnStop = new QPushButton("Stop", this);
    m_btnStop->setStyleSheet("QPushButton { padding: 8px 16px; background-color: #DC3545; border-radius: 4px; color: white; font-weight: bold; } QPushButton:hover { background-color: #E04555; }");
    m_btnStop->setVisible(false);
    connect(m_btnStop, &QPushButton::clicked, this, &ImageTranslationWidget::onStopTranslation);
    
    toolbarLayout->addWidget(m_btnLoad);
    toolbarLayout->addWidget(m_btnSave);
    toolbarLayout->addStretch();
    toolbarLayout->addWidget(m_comboSourceLang);
    toolbarLayout->addWidget(arrowLabel);
    toolbarLayout->addWidget(m_editTargetLang);
    toolbarLayout->addSpacing(20);
    toolbarLayout->addWidget(m_btnTranslate);
    toolbarLayout->addWidget(m_btnTranslateAll);
    toolbarLayout->addWidget(m_btnStop);
    
    mainLayout->addWidget(topToolbar);
    
    // --- MAIN CONTENT (Splitter: Left Sidebar + Image View) ---
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setStyleSheet("QSplitter::handle { background-color: #3E3E3E; }");
    
    // Left Sidebar - Image List
    QWidget *leftSidebar = new QWidget(this);
    leftSidebar->setMinimumWidth(180);
    leftSidebar->setMaximumWidth(300);
    leftSidebar->setStyleSheet("background-color: #252526;");
    QVBoxLayout *sidebarLayout = new QVBoxLayout(leftSidebar);
    sidebarLayout->setContentsMargins(5, 5, 5, 5);
    sidebarLayout->setSpacing(5);
    
    QLabel *sidebarTitle = new QLabel("Images", this);
    sidebarTitle->setStyleSheet("color: #AAAAAA; font-weight: bold; padding: 5px;");
    sidebarLayout->addWidget(sidebarTitle);
    
    m_imageListWidget = new QListWidget(this);
    m_imageListWidget->setStyleSheet(
        "QListWidget { background-color: #1E1E1E; border: 1px solid #3E3E3E; color: white; }"
        "QListWidget::item { padding: 8px; border-bottom: 1px solid #333; }"
        "QListWidget::item:selected { background-color: #094771; }"
        "QListWidget::item:hover { background-color: #2A2D2E; }"
    );
    m_imageListWidget->setIconSize(QSize(40, 40));
    connect(m_imageListWidget, &QListWidget::currentRowChanged, this, &ImageTranslationWidget::onImageSelected);
    sidebarLayout->addWidget(m_imageListWidget, 1);
    
    // Sidebar buttons
    QHBoxLayout *sidebarBtnLayout = new QHBoxLayout();
    m_btnRemove = new QPushButton("Remove", this);
    m_btnRemove->setStyleSheet("QPushButton { padding: 5px 10px; background-color: #444; border-radius: 3px; color: white; } QPushButton:hover { background-color: #555; }");
    connect(m_btnRemove, &QPushButton::clicked, this, &ImageTranslationWidget::onRemoveImage);
    
    m_btnClear = new QPushButton("Clear All", this);
    m_btnClear->setStyleSheet("QPushButton { padding: 5px 10px; background-color: #444; border-radius: 3px; color: white; } QPushButton:hover { background-color: #555; }");
    connect(m_btnClear, &QPushButton::clicked, this, &ImageTranslationWidget::onClearAll);
    
    sidebarBtnLayout->addWidget(m_btnRemove);
    sidebarBtnLayout->addWidget(m_btnClear);
    sidebarLayout->addLayout(sidebarBtnLayout);
    
    splitter->addWidget(leftSidebar);
    
    // Right Content - Image View
    QWidget *rightContent = new QWidget(this);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightContent);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);
    
    m_imageScene = new QGraphicsScene(this);
    m_imageView = new QGraphicsView(m_imageScene);
    m_imageView->setRenderHint(QPainter::Antialiasing);
    m_imageView->setDragMode(QGraphicsView::ScrollHandDrag);
    m_imageView->setStyleSheet("background-color: #1E1E1E; border: none;");
    rightLayout->addWidget(m_imageView, 1);
    
    splitter->addWidget(rightContent);
    splitter->setSizes({200, 600});
    
    mainLayout->addWidget(splitter, 1);
    
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

void ImageTranslationWidget::onAddImages()
{
    QStringList paths = QFileDialog::getOpenFileNames(this, "Add Images", "", "Images (*.png *.jpg *.jpeg *.bmp)");
    
    for (const QString &path : paths) {
        // Avoid duplicates
        bool exists = false;
        for (const ImageItem &item : m_imageQueue) {
            if (item.path == path) {
                exists = true;
                break;
            }
        }
        
        if (!exists) {
            ImageItem item;
            item.path = path;
            item.status = ImageItem::Pending;
            m_imageQueue.append(item);
            
            // Add to list widget with thumbnail
            QFileInfo fi(path);
            QListWidgetItem *listItem = new QListWidgetItem(fi.fileName());
            QPixmap thumb(path);
            if (!thumb.isNull()) {
                listItem->setIcon(QIcon(thumb.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
            }
            listItem->setToolTip(path);
            m_imageListWidget->addItem(listItem);
        }
    }
    
    // Auto-select first if none selected
    if (m_currentQueueIndex < 0 && !m_imageQueue.isEmpty()) {
        m_imageListWidget->setCurrentRow(0);
    }
}

void ImageTranslationWidget::onRemoveImage()
{
    int row = m_imageListWidget->currentRow();
    if (row >= 0 && row < m_imageQueue.size()) {
        m_imageQueue.removeAt(row);
        delete m_imageListWidget->takeItem(row);
        
        if (m_imageQueue.isEmpty()) {
            m_currentQueueIndex = -1;
            m_imageScene->clear();
            m_currentImagePath.clear();
        } else if (row <= m_currentQueueIndex) {
            m_currentQueueIndex = qMax(0, m_currentQueueIndex - 1);
            m_imageListWidget->setCurrentRow(m_currentQueueIndex);
        }
    }
}

void ImageTranslationWidget::onClearAll()
{
    m_imageQueue.clear();
    m_imageListWidget->clear();
    m_currentQueueIndex = -1;
    m_imageScene->clear();
    m_currentImagePath.clear();
    m_detections = QJsonArray();
    m_translatedTexts.clear();
    m_inpaintedImagePath.clear();
    m_statusLabel->setText("Ready.");
}

void ImageTranslationWidget::onImageSelected(int row)
{
    if (row < 0 || row >= m_imageQueue.size()) return;
    
    m_currentQueueIndex = row;
    const ImageItem &item = m_imageQueue[row];
    
    // Load image data from queue
    m_currentImagePath = item.path;
    m_detections = item.detections;
    m_translatedTexts = item.translatedTexts;
    m_inpaintedImagePath = item.inpaintedPath;
    
    // Update view
    if (item.status == ImageItem::Completed && !item.translatedTexts.isEmpty()) {
        m_currentViewMode = Translated;
        m_viewGroup->button(Translated)->setChecked(true);
    } else {
        m_currentViewMode = Original;
        m_viewGroup->button(Original)->setChecked(true);
    }
    
    updateViewMode();
    
    // Update status display
    QString statusStr;
    switch (item.status) {
        case ImageItem::Pending: statusStr = "Pending"; break;
        case ImageItem::Processing: statusStr = "Processing..."; break;
        case ImageItem::Completed: statusStr = "Completed"; break;
        case ImageItem::Error: statusStr = "Error"; break;
    }
    m_statusLabel->setText(QString("Image: %1 [%2]").arg(QFileInfo(item.path).fileName(), statusStr));
}

void ImageTranslationWidget::updateListItemStatus(int index, int status)
{
    if (index < 0 || index >= m_imageListWidget->count()) return;
    
    QListWidgetItem *item = m_imageListWidget->item(index);
    QString emoji;
    switch (status) {
        case ImageItem::Pending: emoji = "🕐 "; break;
        case ImageItem::Processing: emoji = "⏳ "; break;
        case ImageItem::Completed: emoji = "✅ "; break;
        case ImageItem::Error: emoji = "❌ "; break;
    }
    
    QString fileName = QFileInfo(m_imageQueue[index].path).fileName();
    item->setText(emoji + fileName);
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
    }
}

bool ImageTranslationWidget::processImage(int index)
{
    if (index < 0 || index >= m_imageQueue.size()) return false;
    if (d->translator.is_none()) return false;
    
    ImageItem &item = m_imageQueue[index];
    QString imagePath = item.path;
    QString fileName = QFileInfo(imagePath).fileName();
    
    try {
        QString sourceLang = m_comboSourceLang->currentData().toString();
        
        // Step 1: OCR
        m_statusLabel->setText(QString("Processing %1/%2: %3 - OCR...").arg(index + 1).arg(m_imageQueue.size()).arg(fileName));
        QApplication::processEvents();
        if (m_cancelRequested) return false;
        
        std::string resInfo = d->translator.attr("translate_image")(
            imagePath.toStdString(),
            sourceLang.toStdString(),
            "th"
        ).cast<std::string>();
        
        QJsonDocument doc = QJsonDocument::fromJson(QString::fromStdString(resInfo).toUtf8());
        if (!doc.isArray()) {
            item.status = ImageItem::Error;
            return false;
        }
        item.detections = doc.array();
        
        if (item.detections.isEmpty()) {
            item.status = ImageItem::Completed;
            return true; // No text = still success
        }
        
        // Step 2: Inpainting
        m_statusLabel->setText(QString("Processing %1/%2: %3 - Inpainting...").arg(index + 1).arg(m_imageQueue.size()).arg(fileName));
        QApplication::processEvents();
        if (m_cancelRequested) return false;
        
        QString detectionsJson = QString::fromUtf8(QJsonDocument(item.detections).toJson(QJsonDocument::Compact));
        std::string enrichedJsonStr = d->translator.attr("inpaint_text_regions")(
            imagePath.toStdString(),
            detectionsJson.toStdString()
        ).cast<std::string>();
        
        QJsonDocument enrichedDoc = QJsonDocument::fromJson(QString::fromStdString(enrichedJsonStr).toUtf8());
        if (enrichedDoc.isObject()) {
            QJsonObject root = enrichedDoc.object();
            item.inpaintedPath = root["inpainted_path"].toString();
            if (root.contains("detections")) {
                item.detections = root["detections"].toArray();
            }
        }
        
        // Step 3: Translation
        m_statusLabel->setText(QString("Processing %1/%2: %3 - Translating...").arg(index + 1).arg(m_imageQueue.size()).arg(fileName));
        QApplication::processEvents();
        if (m_cancelRequested) return false;
        
        item.translatedTexts.clear();
        QStringList textsToTranslate;
        for (const QJsonValue &val : item.detections) {
            textsToTranslate.append(val.toObject()["text"].toString());
        }
        
        // Synchronous translation for batch (simplified)
        QVariantMap settings;
        settings["targetLanguage"] = m_editTargetLang->text();
        settings["googleApi"] = m_googleApi;
        settings["googleApiKey"] = m_apiKey;
        
        // Store current values for callback
        m_detections = item.detections;
        m_translatedTexts.clear();
        m_currentTranslationIndex = 0;
        m_inpaintedImagePath = item.inpaintedPath;
        m_currentImagePath = imagePath;
        
        m_translationManager->translate("Google Translate", textsToTranslate, settings);
        
        // Wait for translations (blocking event loop processing)
        while (m_currentTranslationIndex < item.detections.size() && !m_cancelRequested) {
            QApplication::processEvents();
        }
        
        if (m_cancelRequested) return false;
        
        // Copy results back to item
        item.translatedTexts = m_translatedTexts;
        item.status = ImageItem::Completed;
        
        return true;
        
    } catch (const std::exception &e) {
        qDebug() << "Process error:" << e.what();
        item.status = ImageItem::Error;
        return false;
    }
}

void ImageTranslationWidget::onTranslate()
{
    if (m_currentQueueIndex < 0 || m_currentQueueIndex >= m_imageQueue.size()) {
        QMessageBox::warning(this, "Warning", "Please select an image first.");
        return;
    }
    
    if (d->translator.is_none()) {
        QMessageBox::critical(this, "Error", "OCR/AI backend not initialized.");
        return;
    }
    
    m_cancelRequested = false;
    m_btnTranslate->setEnabled(false);
    m_btnTranslateAll->setEnabled(false);
    m_btnStop->setVisible(true);
    
    int idx = m_currentQueueIndex;
    m_imageQueue[idx].status = ImageItem::Processing;
    updateListItemStatus(idx, ImageItem::Processing);
    
    bool ok = processImage(idx);
    
    m_imageQueue[idx].status = ok ? ImageItem::Completed : ImageItem::Error;
    updateListItemStatus(idx, m_imageQueue[idx].status);
    
    // Reload data for display
    onImageSelected(idx);
    
    m_btnTranslate->setEnabled(true);
    m_btnTranslateAll->setEnabled(true);
    m_btnStop->setVisible(false);
    m_btnSave->setEnabled(ok);
    m_statusLabel->setText(ok ? "Finished." : "Error during translation.");
}

void ImageTranslationWidget::onTranslateAll()
{
    if (m_imageQueue.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please add images first.");
        return;
    }
    
    if (d->translator.is_none()) {
        QMessageBox::critical(this, "Error", "OCR/AI backend not initialized.");
        return;
    }
    
    m_isBatchProcessing = true;
    m_cancelRequested = false;
    m_btnTranslate->setEnabled(false);
    m_btnTranslateAll->setEnabled(false);
    m_btnStop->setVisible(true);
    
    int completed = 0, errors = 0;
    
    for (int i = 0; i < m_imageQueue.size(); ++i) {
        if (m_cancelRequested) {
            m_statusLabel->setText(QString("Stopped. Completed: %1, Errors: %2").arg(completed).arg(errors));
            break;
        }
        
        if (m_imageQueue[i].status == ImageItem::Pending) {
            m_currentQueueIndex = i;
            m_imageListWidget->setCurrentRow(i);
            
            m_imageQueue[i].status = ImageItem::Processing;
            updateListItemStatus(i, ImageItem::Processing);
            
            bool ok = processImage(i);
            
            if (ok) {
                m_imageQueue[i].status = ImageItem::Completed;
                completed++;
            } else if (!m_cancelRequested) {
                m_imageQueue[i].status = ImageItem::Error;
                errors++;
            }
            updateListItemStatus(i, m_imageQueue[i].status);
            
            QApplication::processEvents();
        }
    }
    
    m_isBatchProcessing = false;
    m_btnTranslate->setEnabled(true);
    m_btnTranslateAll->setEnabled(true);
    m_btnStop->setVisible(false);
    
    if (!m_cancelRequested) {
        m_statusLabel->setText(QString("Batch Complete. Completed: %1, Errors: %2").arg(completed).arg(errors));
    }
    
    // Refresh current view
    if (m_currentQueueIndex >= 0) {
        onImageSelected(m_currentQueueIndex);
    }
}

void ImageTranslationWidget::onStopTranslation()
{
    m_cancelRequested = true;
    m_statusLabel->setText("Stopping...");
}

void ImageTranslationWidget::onTranslationFinished(const qtlingo::TranslationResult &result)
{
    m_translatedTexts.append(result.translatedText);
    m_currentTranslationIndex++;
    
    // Guard against division by zero
    int total = m_detections.size();
    if (total <= 0) {
        // No detections - mark as finished
        m_statusLabel->setText("Status: Finished (no text).");
        m_btnTranslate->setEnabled(true);
        m_btnTranslateAll->setEnabled(true);
        m_btnSave->setEnabled(true);
        return;
    }
    
    int pct = (m_currentTranslationIndex * 100) / total;
    m_statusLabel->setText(QString("Status: Translating... %1%").arg(pct));
    
    if (m_currentTranslationIndex >= total) {
        m_statusLabel->setText("Status: Finished.");
        m_btnTranslate->setEnabled(true);
        m_btnTranslateAll->setEnabled(true);
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
                
                // Calculate text position (centered in bbox)
                int textX = x + (w - fm.horizontalAdvance(translatedText)) / 2;
                int textY = y + (h + fm.ascent() - fm.descent()) / 2;
                
                // Multi-pass shadow/halo drawing technique
                // Draw text multiple times with offset positions for smooth halo effect
                // This avoids the "hollow" issue with QPainterPath stroke on complex glyphs
                
                int haloSize = 3;  // Halo thickness in pixels
                pixmapPainter.setPen(haloColor);
                
                // Draw halo in 8 directions + 4 diagonal midpoints for smooth coverage
                const int offsets[][2] = {
                    {-haloSize, 0}, {haloSize, 0}, {0, -haloSize}, {0, haloSize},  // 4 cardinal
                    {-haloSize, -haloSize}, {haloSize, -haloSize}, {-haloSize, haloSize}, {haloSize, haloSize},  // 4 diagonal
                    {-haloSize/2, -haloSize}, {haloSize/2, -haloSize}, {-haloSize/2, haloSize}, {haloSize/2, haloSize},  // 4 more for smoothness
                    {-haloSize, -haloSize/2}, {haloSize, -haloSize/2}, {-haloSize, haloSize/2}, {haloSize, haloSize/2}   // 4 more
                };
                
                for (const auto& offset : offsets) {
                    pixmapPainter.drawText(textX + offset[0], textY + offset[1], translatedText);
                }
                
                // Draw main text on top
                pixmapPainter.setPen(textColor);
                pixmapPainter.drawText(textX, textY, translatedText);
                
                pixmapPainter.restore();
            }
        }
        
        // Re-add to scene since we modified the pixmap
        m_imageScene->clear();
        m_imageScene->addPixmap(displayPixmap);
    }
}
