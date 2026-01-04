#include "imagetranslationwidget.h"
#include "ui_imagetranslationwidget.h"
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
    , m_imageScene(new QGraphicsScene(this))
    , ui(new Ui::ImageTranslationWidget)
    , d(new Private)
{
    ui->setupUi(this);
    
    // Setup Graphics View
    ui->m_imageView->setScene(m_imageScene);
    
    // Setup View Group (Logic only, buttons are in UI)
    m_viewGroup = new QButtonGroup(this);
    m_viewGroup->setExclusive(true);
    m_viewGroup->addButton(ui->m_btnViewOriginal, Original);
    m_viewGroup->addButton(ui->m_btnViewClean, Clean);
    m_viewGroup->addButton(ui->m_btnViewTranslated, Translated);
    
    connect(m_viewGroup, SIGNAL(idClicked(int)), this, SLOT(onViewModeChanged(int)));
    
    // Repopulate combo box to ensure user data is set (not possible in .ui safely)
    ui->m_comboSourceLang->clear();
    ui->m_comboSourceLang->addItem("English", "en");
    ui->m_comboSourceLang->addItem("Japanese", "ja");
    ui->m_comboSourceLang->addItem("Chinese", "ch_sim");
    ui->m_comboSourceLang->addItem("Korean", "ko");
    
    // Connect UI signals
    connect(ui->m_btnLoad, &QPushButton::clicked, this, &ImageTranslationWidget::onAddImages);
    connect(ui->m_btnSave, &QPushButton::clicked, this, &ImageTranslationWidget::onSaveImage);
    connect(ui->m_btnTranslate, &QPushButton::clicked, this, &ImageTranslationWidget::onTranslate);
    connect(ui->m_btnTranslateAll, &QPushButton::clicked, this, &ImageTranslationWidget::onTranslateAll);
    connect(ui->m_btnStop, &QPushButton::clicked, this, &ImageTranslationWidget::onStopTranslation);
    
    connect(ui->m_btnRemove, &QPushButton::clicked, this, &ImageTranslationWidget::onRemoveImage);
    connect(ui->m_btnClear, &QPushButton::clicked, this, &ImageTranslationWidget::onClearAll);
    connect(ui->m_imageListWidget, &QListWidget::currentRowChanged, this, &ImageTranslationWidget::onImageSelected);
    
    connect(ui->m_btnPeekOriginal, &QPushButton::pressed, this, &ImageTranslationWidget::onPeekPressed);
    connect(ui->m_btnPeekOriginal, &QPushButton::released, this, &ImageTranslationWidget::onPeekReleased);
    
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
            ui->m_statusLabel->setText("Status: EasyOCR not found. Running in Mock Mode.");
            ui->m_statusLabel->setStyleSheet("color: orange;");
        } else if (useGpu) {
            ui->m_statusLabel->setText(QString("Status: Ready (%1)").arg(deviceName));
            ui->m_statusLabel->setStyleSheet("color: #00FF7F;"); // Spring green
        } else {
            // CPU mode - show warning with reason
            ui->m_statusLabel->setText(QString("Status: Ready (CPU Mode) - %1").arg(gpuStatus));
            ui->m_statusLabel->setStyleSheet("color: #FFD700;"); // Gold/yellow warning
        }
        
        qDebug() << "ImageTranslator initialized:" << gpuStatus;
        
    } catch (const std::exception &e) {
        qDebug() << "Failed to init ImageTranslator:" << e.what();
        ui->m_statusLabel->setText(QString("Status: Error init Python: %1").arg(e.what()));
        ui->m_statusLabel->setStyleSheet("color: red;");
        d->translator = py::none();
    }
}

ImageTranslationWidget::~ImageTranslationWidget()
{
    delete d;
    delete ui;
}

void ImageTranslationWidget::setupUi()
{
    // Legacy setupUi removed. Logic moved to .ui file.
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
            ui->m_imageListWidget->addItem(listItem);
        }
    }
    
    // Auto-select first if none selected
    if (m_currentQueueIndex < 0 && !m_imageQueue.isEmpty()) {
        ui->m_imageListWidget->setCurrentRow(0);
    }
}

void ImageTranslationWidget::onRemoveImage()
{
    int row = ui->m_imageListWidget->currentRow();
    if (row >= 0 && row < m_imageQueue.size()) {
        m_imageQueue.removeAt(row);
        delete ui->m_imageListWidget->takeItem(row);
        
        if (m_imageQueue.isEmpty()) {
            m_currentQueueIndex = -1;
            m_imageScene->clear();
            m_currentImagePath.clear();
        } else if (row <= m_currentQueueIndex) {
            m_currentQueueIndex = qMax(0, m_currentQueueIndex - 1);
            ui->m_imageListWidget->setCurrentRow(m_currentQueueIndex);
        }
    }
}

void ImageTranslationWidget::onClearAll()
{
    m_imageQueue.clear();
    ui->m_imageListWidget->clear();
    m_currentQueueIndex = -1;
    m_imageScene->clear();
    m_currentImagePath.clear();
    m_detections = QJsonArray();
    m_translatedTexts.clear();
    m_inpaintedImagePath.clear();
    ui->m_statusLabel->setText("Ready.");
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
    ui->m_statusLabel->setText(QString("Image: %1 [%2]").arg(QFileInfo(item.path).fileName(), statusStr));
}

void ImageTranslationWidget::updateListItemStatus(int index, int status)
{
    if (index < 0 || index >= ui->m_imageListWidget->count()) return;
    
    QListWidgetItem *item = ui->m_imageListWidget->item(index);
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
        ui->m_imageView->setSceneRect(pixmap.rect());
        ui->m_imageView->fitInView(m_imageScene->itemsBoundingRect(), Qt::KeepAspectRatio);
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
        QString sourceLang = ui->m_comboSourceLang->currentData().toString();
        
        // Step 1: OCR
        ui->m_statusLabel->setText(QString("Processing %1/%2: %3 - OCR...").arg(index + 1).arg(m_imageQueue.size()).arg(fileName));
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
        ui->m_statusLabel->setText(QString("Processing %1/%2: %3 - Inpainting...").arg(index + 1).arg(m_imageQueue.size()).arg(fileName));
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
        ui->m_statusLabel->setText(QString("Processing %1/%2: %3 - Translating...").arg(index + 1).arg(m_imageQueue.size()).arg(fileName));
        QApplication::processEvents();
        if (m_cancelRequested) return false;
        
        item.translatedTexts.clear();
        QStringList textsToTranslate;
        for (const QJsonValue &val : item.detections) {
            textsToTranslate.append(val.toObject()["text"].toString());
        }
        
        // Synchronous translation for batch (simplified)
        QVariantMap settings;
        settings["targetLanguage"] = ui->m_editTargetLang->text();
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
    ui->m_btnTranslate->setEnabled(false);
    ui->m_btnTranslateAll->setEnabled(false);
    ui->m_btnStop->setVisible(true);
    
    int idx = m_currentQueueIndex;
    m_imageQueue[idx].status = ImageItem::Processing;
    updateListItemStatus(idx, ImageItem::Processing);
    
    bool ok = processImage(idx);
    
    m_imageQueue[idx].status = ok ? ImageItem::Completed : ImageItem::Error;
    updateListItemStatus(idx, m_imageQueue[idx].status);
    
    // Reload data for display
    onImageSelected(idx);
    
    ui->m_btnTranslate->setEnabled(true);
    ui->m_btnTranslateAll->setEnabled(true);
    ui->m_btnStop->setVisible(false);
    ui->m_btnSave->setEnabled(ok);
    ui->m_statusLabel->setText(ok ? "Finished." : "Error during translation.");
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
    ui->m_btnTranslate->setEnabled(false);
    ui->m_btnTranslateAll->setEnabled(false);
    ui->m_btnStop->setVisible(true);
    
    int completed = 0, errors = 0;
    
    for (int i = 0; i < m_imageQueue.size(); ++i) {
        if (m_cancelRequested) {
            ui->m_statusLabel->setText(QString("Stopped. Completed: %1, Errors: %2").arg(completed).arg(errors));
            break;
        }
        
        if (m_imageQueue[i].status == ImageItem::Pending) {
            m_currentQueueIndex = i;
            ui->m_imageListWidget->setCurrentRow(i);
            
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
    ui->m_btnTranslate->setEnabled(true);
    ui->m_btnTranslateAll->setEnabled(true);
    ui->m_btnStop->setVisible(false);
    
    if (!m_cancelRequested) {
        ui->m_statusLabel->setText(QString("Batch Complete. Completed: %1, Errors: %2").arg(completed).arg(errors));
    }
    
    // Refresh current view
    if (m_currentQueueIndex >= 0) {
        onImageSelected(m_currentQueueIndex);
    }
}

void ImageTranslationWidget::onStopTranslation()
{
    m_cancelRequested = true;
    ui->m_statusLabel->setText("Stopping...");
}

void ImageTranslationWidget::onTranslationFinished(const qtlingo::TranslationResult &result)
{
    m_translatedTexts.append(result.translatedText);
    m_currentTranslationIndex++;
    
    // Guard against division by zero
    int total = m_detections.size();
    if (total <= 0) {
        // No detections - mark as finished
        ui->m_statusLabel->setText("Status: Finished (no text).");
        ui->m_btnTranslate->setEnabled(true);
        ui->m_btnTranslateAll->setEnabled(true);
        ui->m_btnSave->setEnabled(true);
        return;
    }
    
    int pct = (m_currentTranslationIndex * 100) / total;
    ui->m_statusLabel->setText(QString("Status: Translating... %1%").arg(pct));
    
    if (m_currentTranslationIndex >= total) {
        ui->m_statusLabel->setText("Status: Finished.");
        ui->m_btnTranslate->setEnabled(true);
        ui->m_btnTranslateAll->setEnabled(true);
        ui->m_btnSave->setEnabled(true);
        
        // Auto-switch to Translated View on finish
        m_currentViewMode = Translated;
        m_viewGroup->button(Translated)->setChecked(true);
        updateViewMode();
    }
}

void ImageTranslationWidget::onTranslationError(const QString &message)
{
     ui->m_statusLabel->setText("Translation Error: " + message);
     ui->m_btnTranslate->setEnabled(true);
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
        ui->m_statusLabel->setText("Saved to: " + savePath);
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
