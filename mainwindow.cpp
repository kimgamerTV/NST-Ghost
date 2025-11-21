#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "pluginmanagerdialog.h"

#ifdef HAS_LUA
#include "src/plugins/LuaScriptManager.h"
#endif

#include <QStandardItemModel>
#include <QFileIconProvider>
#include <QSplitter>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonObject>
#include <QFileInfo>
#include <QMenu>
#include <QShortcut>
#include <QSettings>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QStyle>
#include <QApplication>
#include <QScreen>
#include <QtConcurrent>

#include "customprogressdialog.h"
#include "loadprojectdialog.h"
#include <QRegularExpression>
#include "projectdatamanager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_progressDialog(nullptr)
    , m_spinnerTimer(new QTimer(this))
{
    QString logFilePath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/mainwindow_log.txt";
    QFile logFile(logFilePath);
    ui->setupUi(this);
    setWindowIcon(QIcon(":/icons/icon-app.png"));
    resize(1024, 768); // Set a reasonable default size

    m_fileListModel = new QStandardItemModel(this);
    ui->fileListView->setModel(m_fileListModel);

    // Set icon provider
    QFileIconProvider *iconProvider = new QFileIconProvider();
    ui->fileListView->setIconSize(QSize(24, 24));
    ui->fileListView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->fileListView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->fileListView, &QListView::customContextMenuRequested, this, &MainWindow::onFileListCustomContextMenuRequested);

    m_translationModel = new QStandardItemModel(this);
    ui->translationTableView->setModel(m_translationModel);

    // ===== ปรับการแสดงผลตาราง =====
    ui->translationTableView->setWordWrap(true);
    ui->translationTableView->setTextElideMode(Qt::ElideNone);
    ui->translationTableView->setAlternatingRowColors(true);

    // ปรับ Vertical Header
    ui->translationTableView->verticalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->translationTableView->verticalHeader()->setDefaultSectionSize(60); // ความสูงขั้นต่ำ

    // ปรับ Horizontal Header
    ui->translationTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    ui->translationTableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->translationTableView->horizontalHeader()->setStretchLastSection(true);

    // ตั้งชื่อ Header
    m_translationModel->setHorizontalHeaderLabels(QStringList() << "Source Text" << "Translation");

    // กำหนดความกว้างเริ่มต้นของคอลัมน์ Source Text
    ui->translationTableView->setColumnWidth(0, 400);

    // ปรับ Selection behavior
    ui->translationTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->translationTableView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    // เพิ่ม Margins สำหรับเซลล์
    ui->translationTableView->setShowGrid(true);
    ui->translationTableView->setGridStyle(Qt::SolidLine);

    // Set initial splitter sizes
    ui->splitter->setSizes(QList<int>() << 250 << 774);

    // Setup Project Data Manager
    m_projectDataManager = new ProjectDataManager(m_fileListModel, m_translationModel, this);
    connect(m_projectDataManager, &ProjectDataManager::processingFinished, this, &MainWindow::onProjectProcessingFinished);

    // Setup search controller
    m_searchController = new SearchController(m_translationModel, ui->translationTableView, this);
    m_searchController->setTranslationModel(m_translationModel);
    m_searchController->setLoadedGameProjectData(&m_projectDataManager->getLoadedGameProjectData());
    m_searchController->setFileListModel(m_fileListModel);

    // Setup search dialog
    m_searchDialog = new SearchDialog(this);
    connect(m_searchDialog, &SearchDialog::searchRequested, this, &MainWindow::onSearchRequested);
    connect(m_searchDialog, &SearchDialog::resultSelected, this, &MainWindow::onSearchResultSelected);
    connect(m_searchDialog->lineEdit(), &QLineEdit::textChanged, m_searchController, &SearchController::onSearchQueryChanged);

    // Setup shortcut controller
    m_shortcutController = new ShortcutController(this);
    m_shortcutController->createShortcuts();
    connect(m_shortcutController, &ShortcutController::focusSearch, this, &MainWindow::openSearchDialog);
    m_shortcutController->createSelectAllShortcut(ui->translationTableView);
    connect(m_shortcutController, &ShortcutController::selectAllRequested, this, &MainWindow::onSelectAllRequested);

    // Setup BGA Data Manager
    m_bgaDataManager = new BGADataManager(this);
    connect(m_bgaDataManager, &BGADataManager::errorOccurred, this, &MainWindow::onBGADataError);
    connect(m_bgaDataManager, &BGADataManager::fontsLoaded, this, &MainWindow::onFontsLoaded);
    connect(m_bgaDataManager, &BGADataManager::progressUpdated, this, [this](int value, const QString &message) {
        if (m_progressDialog) {
            m_progressDialog->setValue(value);
            m_progressDialog->setLabelText(message);
        }
    });

    // Setup Translation Service Manager
    m_translationServiceManager = new TranslationServiceManager(this);
    connect(m_translationServiceManager, &TranslationServiceManager::translationFinished, this, &MainWindow::onTranslationFinished);
    connect(m_translationServiceManager, &TranslationServiceManager::errorOccurred, this, &MainWindow::onTranslationServiceError);
    connect(m_translationServiceManager, &TranslationServiceManager::progressUpdated, this, [this](int current, int total) {
        if (total == 0) return;

        statusBar()->showMessage(QString("Translating: %1/%2").arg(current).arg(total));
        
        if (current == 1 && m_currentTranslatingFileIndex.isValid()) {
            m_spinnerTimer->start(300);
        }
        
        if (current >= total) {
            m_spinnerTimer->stop();
            
            if (m_currentTranslatingFileIndex.isValid()) {
                QStandardItem *item = m_fileListModel->itemFromIndex(m_currentTranslatingFileIndex);
                if (item) {
                    QString originalText = item->data(Qt::UserRole + 1).toString();
                    if (!originalText.isEmpty()) {
                        item->setText("✓ " + originalText);
                        item->setData(QVariant(), Qt::UserRole + 1);
                    }
                }
            }
            m_isTranslating = false;
            processNextTranslationJob();
            statusBar()->showMessage("Translation finished.", 4000);
            ui->fileListView->setEnabled(true);
            ui->translationTableView->setEnabled(true);
        }
    });
    
    // Setup spinner animation
    connect(m_spinnerTimer, &QTimer::timeout, this, [this]() {
        if (!m_currentTranslatingFileIndex.isValid()) return;
        
        QStandardItem *item = m_fileListModel->itemFromIndex(m_currentTranslatingFileIndex);
        if (!item) return;
        
        static const QStringList spinners = {"⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"};
        m_spinnerFrame = (m_spinnerFrame + 1) % spinners.size();
        
        QString originalText = item->data(Qt::UserRole + 1).toString();
        if (originalText.isEmpty()) {
            originalText = item->text();
            item->setData(originalText, Qt::UserRole + 1);
        }
        
        item->setText(spinners[m_spinnerFrame] + " " + originalText);
    });

    // Setup UI batch update timer
    m_uiUpdateTimer = new QTimer(this);
    m_uiUpdateTimer->setInterval(100); // Update every 100ms
    m_uiUpdateTimer->setSingleShot(true);
    connect(m_uiUpdateTimer, &QTimer::timeout, this, [this]() {
        if (m_pendingUIUpdates.isEmpty()) return;
        
        ui->translationTableView->setUpdatesEnabled(false);
        int minRow = INT_MAX;
        int maxRow = 0;
        for (const QModelIndex &idx : m_pendingUIUpdates) {
            if (idx.isValid() && idx.row() >= 0) {
                if (idx.row() < minRow) minRow = idx.row();
                if (idx.row() > maxRow) maxRow = idx.row();
            }
        }
        
        if (minRow <= maxRow && minRow != INT_MAX) {
            QModelIndex topLeft = m_translationModel->index(minRow, 1);
            QModelIndex bottomRight = m_translationModel->index(maxRow, 1);
            if (topLeft.isValid() && bottomRight.isValid()) {
                emit m_translationModel->dataChanged(topLeft, bottomRight);
            }
        }
        ui->translationTableView->setUpdatesEnabled(true);
        
        m_pendingUIUpdates.clear();
    });

    // Setup result processing timer (for batching incoming translations)
    m_resultProcessingTimer = new QTimer(this);
    m_resultProcessingTimer->setInterval(100); // Process every 100ms
    connect(m_resultProcessingTimer, &QTimer::timeout, this, &MainWindow::processIncomingResults);


    m_menuBar = new MenuBar(this);
    setMenuBar(m_menuBar);

    connect(m_menuBar, &MenuBar::openMockData, this, &MainWindow::onOpenMockData);
    connect(m_menuBar, &MenuBar::loadFromGameProject, this, &MainWindow::onLoadFromGameProject);
    connect(m_menuBar, &MenuBar::settings, this, &MainWindow::onSettingsActionTriggered);
    connect(m_menuBar, &MenuBar::saveProject, this, &MainWindow::onSaveGameProject);
    connect(m_menuBar, &MenuBar::exit, this, &QMainWindow::close);
    connect(m_menuBar, &MenuBar::fontManager, this, &MainWindow::onFontManagerActionTriggered);
    connect(m_menuBar, &MenuBar::pluginManager, this, &MainWindow::onPluginManagerActionTriggered);

    // Enable custom context menu for translation table view
    ui->translationTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->translationTableView, &QTableView::customContextMenuRequested, this, &MainWindow::onTranslationTableViewCustomContextMenuRequested);

    // Connect dataChanged signal to update m_loadedGameProjectData
    connect(m_translationModel, &QStandardItemModel::dataChanged, this, &MainWindow::onTranslationDataChanged);

    loadSettings();

    m_updateController = new UpdateController(this);
    m_updateController->checkForUpdates();

    // ใช้ QFutureWatcher เพื่อจัดการการโหลดแบบ async
    connect(&m_loadFutureWatcher, &QFutureWatcher<QJsonArray>::finished, this, &MainWindow::onLoadingFinished);

#ifdef HAS_LUA
    // Load enabled Lua plugins only
    QString scriptPath = QCoreApplication::applicationDirPath() + "/scripts";
    QDir scriptDir(scriptPath);
    QSettings settings;
    
    int loadedCount = 0;
    for (const QString& file : scriptDir.entryList({"*.lua"}, QDir::Files)) {
        bool enabled = settings.value("plugins/" + file + "/enabled", false).toBool();
        if (enabled) {
            // Load only this plugin
            LuaScriptManager::instance().loadScriptsFromDir(scriptPath);
            LuaScriptManager::instance().registerAPI();
            qDebug() << "Loaded enabled plugin:" << file;
            loadedCount++;
        }
    }
    
    // Connect Lua signals
    connect(&LuaScriptManager::instance(), &LuaScriptManager::mockDataLoaded,
            this, &MainWindow::onMockDataLoaded);
    
    if (loadedCount > 0) {
        qDebug() << "Total enabled plugins loaded:" << loadedCount;
    } else {
        qDebug() << "No plugins enabled. Use Tools > Plugin Manager to enable plugins.";
    }
#endif
    connect(ui->fileListView, &QListView::clicked, m_projectDataManager, &ProjectDataManager::onFileSelected);
}

void MainWindow::onLoadingFinished()
{
    if (m_progressDialog) {
        m_progressDialog->close();
        delete m_progressDialog;
        m_progressDialog = nullptr;
    }

    QJsonArray extractedTextsArray = m_loadFutureWatcher.result();
    m_projectDataManager->onLoadingFinished(extractedTextsArray);
}

void MainWindow::onProjectProcessingFinished()
{
    if (m_fileListModel->rowCount() > 0) {
        // Auto-select first file
        QModelIndex firstIndex = m_fileListModel->index(0, 0);
        ui->fileListView->setCurrentIndex(firstIndex);
        // Call onFileSelected directly on ProjectDataManager
        m_projectDataManager->onFileSelected(firstIndex);
    }
}

MainWindow::~MainWindow()
{
    m_uiUpdateTimer->stop();
    delete ui;
}

void MainWindow::onLoadFromGameProject()
{
    QStringList availableEngines = m_bgaDataManager->getAvailableAnalyzers();
    if (availableEngines.isEmpty()) {
        QMessageBox::warning(this, "Error", "No game analyzers available.");
        return;
    }

    LoadProjectDialog dialog(availableEngines, this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QString engineName = dialog.selectedEngine();
    QString projectPath = dialog.projectPath();

    if (engineName.isEmpty() || projectPath.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "Please select both engine and project path.");
        return;
    }

    m_currentEngineName = engineName;
    m_currentProjectPath = projectPath;

    m_projectDataManager->getLoadedGameProjectData().clear();
    m_fileListModel->clear();

    m_progressDialog = new CustomProgressDialog(this);
    m_progressDialog->setWindowModality(Qt::WindowModal);
    m_progressDialog->setValue(0);
    m_progressDialog->show();

    // เริ่มการโหลดใน thread อื่น
    QFuture<QJsonArray> future = QtConcurrent::run([this, engineName, projectPath]() {
        return m_bgaDataManager->loadStringsFromGameProject(engineName, projectPath);
    });

    m_loadFutureWatcher.setFuture(future);
}

void MainWindow::onBGADataError(const QString &message)
{
    if (m_progressDialog) {
        m_progressDialog->close();
        delete m_progressDialog;
        m_progressDialog = nullptr;
    }
    QMessageBox::critical(this, "Load Error", message);
}

void MainWindow::openSearchDialog()
{
    m_searchDialog->show();
    m_searchDialog->raise();
    m_searchDialog->activateWindow();
    m_searchDialog->lineEdit()->setFocus();
}

void MainWindow::onSearchResultSelected(const QString &fileName, int row)
{
    // Find and select file
    QString fullPath;
    for (const QString &key : m_projectDataManager->getLoadedGameProjectData().keys()) {
        if (QFileInfo(key).fileName() == fileName) {
            fullPath = key;
            break;
        }
    }

    if (fullPath.isEmpty()) return;

    // Select file in list
    for (int i = 0; i < m_fileListModel->rowCount(); ++i) {
        QStandardItem *item = m_fileListModel->item(i);
        if (item && item->data(Qt::UserRole).toString() == fullPath) {
            QModelIndex modelIdx = m_fileListModel->index(i, 0);
            ui->fileListView->setCurrentIndex(modelIdx);
            ui->translationTableView->setUpdatesEnabled(false);
            m_projectDataManager->onFileSelected(modelIdx);
            ui->translationTableView->setUpdatesEnabled(true);
            break;
        }
    }

    // Select row in table
    if (row >= 0 && row < m_translationModel->rowCount()) {
        QModelIndex tableIdx = m_translationModel->index(row, 0);
        ui->translationTableView->scrollTo(tableIdx);
        ui->translationTableView->selectRow(row);
    }
}

void MainWindow::onOpenMockData()
{
    // Mock data for testing
    QJsonArray mockArray;
    QJsonObject obj1, obj2;
    obj1["file"] = "script1.json";
    obj1["source"] = "Hello, world!";
    obj1["text"] = "";
    obj1["key"] = "greeting_001";

    obj2["file"] = "script1.json";
    obj2["source"] = "Goodbye!";
    obj2["text"] = "¡Adiós!";
    obj2["key"] = "farewell_001";

    mockArray.append(obj1);
    mockArray.append(obj2);

    // Simulate loading
    m_projectDataManager->getLoadedGameProjectData().clear();
    m_fileListModel->clear();

    QMap<QString, QJsonArray> fileMap;
    fileMap["script1.json"] = mockArray;
    m_projectDataManager->getLoadedGameProjectData() = fileMap;
    
    QStandardItem *item = new QStandardItem("script1.json");
    item->setData("script1.json", Qt::UserRole);
    m_fileListModel->appendRow(item);

    QModelIndex idx = m_fileListModel->index(0, 0);
    ui->fileListView->setCurrentIndex(idx);
    ui->translationTableView->setUpdatesEnabled(false);
    m_projectDataManager->onFileSelected(idx);
    ui->translationTableView->setUpdatesEnabled(true);
}

void MainWindow::onMockDataLoaded(const QJsonArray &data)
{
    // Load mock data from Lua plugin
    m_projectDataManager->getLoadedGameProjectData().clear();
    m_fileListModel->clear();

    QMap<QString, QJsonArray> fileMap;
    QJsonArray mockArray;
    
    for (const auto& item : data) {
        QJsonObject obj = item.toObject();
        QJsonObject entry;
        entry["file"] = "mock_data.json";
        entry["source"] = obj["source"];
        entry["text"] = obj["translation"];
        entry["key"] = "mock_" + QString::number(mockArray.size());
        mockArray.append(entry);
    }
    
    fileMap["mock_data.json"] = mockArray;
    m_projectDataManager->getLoadedGameProjectData() = fileMap;
    
    QStandardItem *item = new QStandardItem("mock_data.json");
    item->setData("mock_data.json", Qt::UserRole);
    m_fileListModel->appendRow(item);

    QModelIndex idx = m_fileListModel->index(0, 0);
    ui->fileListView->setCurrentIndex(idx);
    ui->translationTableView->setUpdatesEnabled(false);
    m_projectDataManager->onFileSelected(idx);
    ui->translationTableView->setUpdatesEnabled(true);
}

void MainWindow::onSearchRequested(const QString &query)
{
    m_searchController->onSearchQueryChanged(query);
}

void MainWindow::onTranslationFinished(const qtlingo::TranslationResult &result)
{
    if (!m_translationModel) return;
    
    if (!m_translationModel) return;
    
    QueuedTranslationResult queuedResult;
    queuedResult.result = result;
    
    // Capture the file path associated with this translation
    // If we are in batch mode, use the current translating file
    if (m_currentTranslatingFileIndex.isValid()) {
        QStandardItem *fileItem = m_fileListModel->itemFromIndex(m_currentTranslatingFileIndex);
        if (fileItem) {
            queuedResult.filePath = fileItem->data(Qt::UserRole).toString();
        }
    } else {
        // Fallback to currently loaded file if not batch translating (single file mode)
        queuedResult.filePath = m_projectDataManager->getCurrentLoadedFilePath();
    }
    
    m_incomingResults.enqueue(queuedResult);
    
    if (!m_resultProcessingTimer->isActive()) {
        m_resultProcessingTimer->start();
    }
}

void MainWindow::processIncomingResults()
{
    if (m_incomingResults.isEmpty()) {
        m_resultProcessingTimer->stop();
        return;
    }

    // Process a batch of results (e.g., up to 50 at a time to keep UI responsive)
    int processedCount = 0;
    const int BATCH_SIZE = 50;
    
    // Block signals to prevent excessive redraws
    if (m_translationModel) m_translationModel->blockSignals(true);

    while (!m_incomingResults.isEmpty() && processedCount < BATCH_SIZE) {
        QueuedTranslationResult queuedResult = m_incomingResults.dequeue();
        processedCount++;

        const QString &sourceText = queuedResult.result.sourceText;
        const QString &translatedText = queuedResult.result.translatedText;
        const QString &targetFilePath = queuedResult.filePath;
        
        // 1. Update the UI if the file is currently open
        QString currentLoadedPath = m_projectDataManager->getCurrentLoadedFilePath();
        
        if (targetFilePath == currentLoadedPath) {
            auto pendingList = m_pendingTranslations.values(sourceText);
            for (const PendingTranslation &pending : pendingList) {
                if (pending.filePath != targetFilePath) continue;
                if (!pending.index.isValid()) continue;
                if (pending.index.model() != m_translationModel) continue;
                
                int row = pending.index.row();
                int col = pending.index.column();
                
                if (row < 0 || row >= m_translationModel->rowCount()) continue;
                if (col < 0 || col >= m_translationModel->columnCount()) continue;
                
                QStandardItem *item = m_translationModel->item(row, col);
                if (item) {
                    item->setText(translatedText); 
                    m_pendingUIUpdates.append(pending.index);
                }
            }
        }
        
        // 2. Update the underlying data (Batch Mode support)
        // We use the captured targetFilePath, so even if m_currentTranslatingFileIndex has moved on,
        // we update the correct file's data.
        if (!targetFilePath.isEmpty() && m_projectDataManager->getLoadedGameProjectData().contains(targetFilePath)) {
            QJsonArray textsArray = m_projectDataManager->getLoadedGameProjectData().value(targetFilePath);
            bool modified = false;
            for (int i = 0; i < textsArray.size(); ++i) {
                QJsonObject textObject = textsArray.at(i).toObject();
                if (textObject["source"].toString() == sourceText) {
                    textObject["text"] = translatedText;
                    textsArray.replace(i, textObject);
                    modified = true;
                }
            }
            if (modified) {
                m_projectDataManager->getLoadedGameProjectData().insert(targetFilePath, textsArray);
                
                // If this file is currently open, we might need to refresh the view if it wasn't caught by m_pendingTranslations
                if (targetFilePath == currentLoadedPath) {
                     for(int r=0; r<m_translationModel->rowCount(); ++r) {
                         if (m_translationModel->data(m_translationModel->index(r, 0)).toString() == sourceText) {
                             m_translationModel->setData(m_translationModel->index(r, 1), translatedText);
                         }
                     }
                }
            }
        }
        
        m_pendingTranslations.remove(sourceText);
    }

    if (m_translationModel) m_translationModel->blockSignals(false);

    // Trigger UI update for the changed range
    if (!m_pendingUIUpdates.isEmpty()) {
        if (!m_uiUpdateTimer->isActive()) {
            m_uiUpdateTimer->start();
        }
    }
}

void MainWindow::onTranslationServiceError(const QString &message)
{
    statusBar()->showMessage(QString("Translation Error: %1").arg(message), 5000); // Show for 5 seconds
    qWarning() << "Translation Service Error:" << message;
}

void MainWindow::onTranslationTableViewCustomContextMenuRequested(const QPoint &pos)
{
    QMenu contextMenu(this);

    QAction *translateAction = contextMenu.addAction("Translate Selected with...");
    QAction *translateAllAction = contextMenu.addAction("Translate All Selected");
    QAction *undoAction = contextMenu.addAction("Undo Translation");
    contextMenu.addSeparator();
    QAction *selectAllAction = contextMenu.addAction("Select All");

    connect(translateAction, &QAction::triggered, this, &MainWindow::onTranslateSelectedTextWithService);
    connect(translateAllAction, &QAction::triggered, this, &MainWindow::onTranslateAllSelectedText);
    connect(undoAction, &QAction::triggered, this, &MainWindow::onUndoTranslation);
    connect(selectAllAction, &QAction::triggered, this, &MainWindow::onSelectAllRequested);

    contextMenu.exec(ui->translationTableView->mapToGlobal(pos));
}

void MainWindow::onTranslateSelectedTextWithService()
{
    QModelIndexList selectedIndexes = ui->translationTableView->selectionModel()->selectedIndexes();
    if (selectedIndexes.isEmpty()) {
        QMessageBox::information(this, "Translate", "Please select rows to translate.");
        return;
    }

    QStringList availableServices = m_translationServiceManager->getAvailableServices();
    if (availableServices.isEmpty()) {
        QMessageBox::warning(this, "Error", "No translation services available.");
        return;
    }

    bool ok;
    QString serviceName = QInputDialog::getItem(this, "Translate Selected Text",
                                                "Select Translation Service:", availableServices, 0, false, &ok);
    if (!ok || serviceName.isEmpty()) {
        return;
    }

    QStringList sourceTexts;
    int skippedCount = 0;
    for (const QModelIndex &selectedIndex : selectedIndexes) {
        if (selectedIndex.column() != 0) continue;
        QString sourceText = m_translationModel->data(selectedIndex, Qt::DisplayRole).toString();
        if (!sourceText.isEmpty()) {
            if (isLikelyCode(sourceText)) {
                skippedCount++;
                continue;
            }
            sourceTexts.append(sourceText);
            PendingTranslation pending;
            pending.index = m_translationModel->index(selectedIndex.row(), 1);
            pending.filePath = m_projectDataManager->getCurrentLoadedFilePath();
            m_pendingTranslations.insert(sourceText, pending);
        }
    }

    if (skippedCount > 0) {
        statusBar()->showMessage(QString("Skipped %1 lines (suspected to be code).").arg(skippedCount), 4000);
    }

    if (!sourceTexts.isEmpty()) {
        ui->fileListView->setEnabled(false);
        ui->translationTableView->setEnabled(false);
        
        QVariantMap settings;
        settings["googleApiKey"] = m_apiKey;
        settings["targetLanguage"] = m_targetLanguage;
        settings["googleApi"] = m_googleApi;
        settings["llmProvider"] = m_llmProvider;
        settings["llmApiKey"] = m_llmApiKey;
        settings["llmModel"] = m_llmModel;
        settings["llmBaseUrl"] = m_llmBaseUrl;

        TranslationJob job;
        job.serviceName = serviceName;
        job.sourceTexts = sourceTexts;
        job.settings = settings;
        job.fileIndex = ui->fileListView->currentIndex();
        
        // Mark as queued
        QStandardItem *item = m_fileListModel->itemFromIndex(job.fileIndex);
        if (item) {
            QString originalText = item->data(Qt::UserRole + 1).toString();
            if (originalText.isEmpty()) {
                item->setData(item->text(), Qt::UserRole + 1);
            }
            item->setText("⏳ " + item->data(Qt::UserRole + 1).toString());
        }
        
        m_translationQueue.enqueue(job);
        processNextTranslationJob();
    }
}


void MainWindow::onTranslateAllSelectedText()
{
    // Same as above but use default service
    // You can extend this later
    onTranslateSelectedTextWithService();
}

void MainWindow::onSelectAllRequested()
{
    ui->translationTableView->selectAll();
}

void MainWindow::onSettingsActionTriggered()
{
    SettingsDialog dialog(this);
    dialog.setGoogleApiKey(m_apiKey);
    dialog.setTargetLanguage(m_targetLanguage);
    dialog.setGoogleApi(m_googleApi);
    dialog.setLlmProvider(m_llmProvider);
    dialog.setLlmApiKey(m_llmApiKey);
    dialog.setLlmModel(m_llmModel);
    dialog.setLlmBaseUrl(m_llmBaseUrl);

    QFile file(":/style.qss");
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&file);
        QString styleSheet = stream.readAll();
        dialog.setStyleSheet(styleSheet);
        file.close();
    }

    if (dialog.exec() == QDialog::Accepted) {
        m_apiKey = dialog.googleApiKey();
        m_targetLanguage = dialog.targetLanguage();
        m_targetLanguageName = dialog.targetLanguageName();
        m_googleApi = dialog.isGoogleApi();
        m_llmProvider = dialog.llmProvider();
        m_llmApiKey = dialog.llmApiKey();
        m_llmModel = dialog.llmModel();
        m_llmBaseUrl = dialog.llmBaseUrl();
        saveSettings();
    }
}

void MainWindow::loadSettings()
{
    QSettings settings("MySoft", "NST");
    m_apiKey = settings.value("googleApiKey").toString();
    m_targetLanguage = settings.value("targetLanguage", "es").toString();
    m_targetLanguageName = settings.value("targetLanguageName", "Spanish").toString();
    m_googleApi = settings.value("googleApi", false).toBool();
    m_llmProvider = settings.value("llmProvider", "OpenAI").toString();
    m_llmApiKey = settings.value("llmApiKey").toString();
    m_llmModel = settings.value("llmModel").toString();
    m_llmBaseUrl = settings.value("llmBaseUrl").toString();
}

void MainWindow::saveSettings()
{
    QSettings settings("MySoft", "NST");
    settings.setValue("googleApiKey", m_apiKey);
    settings.setValue("targetLanguage", m_targetLanguage);
    settings.setValue("targetLanguageName", m_targetLanguageName);
    settings.setValue("googleApi", m_googleApi);
    settings.setValue("llmProvider", m_llmProvider);
    settings.setValue("llmApiKey", m_llmApiKey);
    settings.setValue("llmModel", m_llmModel);
    settings.setValue("llmBaseUrl", m_llmBaseUrl);
}

void MainWindow::onFontsLoaded(const QJsonArray &fonts)
{
    m_gameFonts = fonts;
}

void MainWindow::onFontManagerActionTriggered()
{
    FontManagerDialog dialog(m_gameFonts, m_targetLanguageName, this);
    if (dialog.exec() == QDialog::Accepted) {
        // Handle accepted dialog
    }
}

void MainWindow::onPluginManagerActionTriggered()
{
    PluginManagerDialog dialog(this);
    dialog.exec();
}

void MainWindow::onUndoTranslation()
{
    QModelIndexList selectedIndexes = ui->translationTableView->selectionModel()->selectedRows();

    for (const QModelIndex &selectedIndex : selectedIndexes) {
        QModelIndex translationIndex = m_translationModel->index(selectedIndex.row(), 1);
        m_translationModel->setData(translationIndex, "");
    }
}

void MainWindow::onSaveGameProject()
{
    if (m_projectDataManager->getLoadedGameProjectData().isEmpty()) {
        QMessageBox::information(this, "Save Project", "No project data to save.");
        return;
    }

    if (m_bgaDataManager->saveStringsToGameProject(m_currentEngineName, m_currentProjectPath, m_projectDataManager->getLoadedGameProjectData())) {
        QMessageBox::information(this, "Save Project", "Project saved successfully.");
    } else {
        QMessageBox::critical(this, "Save Project", "Failed to save project.");
    }
}

void MainWindow::processNextTranslationJob()
{
    if (m_isTranslating || m_translationQueue.isEmpty()) {
        if (m_translationQueue.isEmpty()) {
            statusBar()->showMessage("All translations completed", 3000);
        }
        return;
    }
    
    m_isTranslating = true;
    TranslationJob job = m_translationQueue.dequeue();
    m_currentTranslatingFileIndex = job.fileIndex;
    
    // Change queued icon to processing
    QStandardItem *item = m_fileListModel->itemFromIndex(job.fileIndex);
    if (item) {
        QString originalText = item->data(Qt::UserRole + 1).toString();
        if (!originalText.isEmpty()) {
            item->setText("▶ " + originalText);
        }
    }
    
    m_translationServiceManager->translate(job.serviceName, job.sourceTexts, job.settings);
}

void MainWindow::onTranslationDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    if (topLeft.column() != 1) return; // Only care about translation column

    for (int row = topLeft.row(); row <= bottomRight.row(); ++row) {
        QString newTranslation = m_translationModel->data(m_translationModel->index(row, 1), Qt::DisplayRole).toString();
        QString sourceText = m_translationModel->data(m_translationModel->index(row, 0), Qt::DisplayRole).toString();
        QString key = m_translationModel->data(m_translationModel->index(row, 0), Qt::UserRole + 1).toString();

        if (m_projectDataManager->getLoadedGameProjectData().contains(m_projectDataManager->getCurrentLoadedFilePath())) {
            QJsonArray textsArray = m_projectDataManager->getLoadedGameProjectData().value(m_projectDataManager->getCurrentLoadedFilePath());
            for (int i = 0; i < textsArray.size(); ++i) {
                QJsonObject textObject = textsArray.at(i).toObject();
                if (textObject["source"].toString() == sourceText && textObject["key"].toString() == key) {
                    textObject["text"] = newTranslation;
                    textsArray.replace(i, textObject);
                    break;
                }
            }
            m_projectDataManager->getLoadedGameProjectData().insert(m_projectDataManager->getCurrentLoadedFilePath(), textsArray);
        }
    }
}

bool MainWindow::isLikelyCode(const QString &text) const
{
    // Heuristic 1: Check for common code operators and symbols.
    // Matches: ==, !=, >=, <=, +=, -=, *=, /=, ->, $, [, ], {, }
    static const QRegularExpression codeRegex(R"([<>=!+\-*\/%]=|\$|->|\[|\]|\{|\})");
    if (text.contains(codeRegex)) {
        return true;
    }

    // Heuristic 2: Contains no Unicode letters at all (is just numbers/punctuation)
    static const QRegularExpression hasLettersRegex(R"(\p{L})");
    if (!text.contains(hasLettersRegex)) {
        return true; // It's likely code/data if it has no letters of any language
    }
    
    // Heuristic 3: Very few spaces, but multiple words and mixed case (like a variableName)
    if (text.count(' ') < 2) {
        static const QRegularExpression mixedCase(R"([a-z]+[A-Z]+)");
        if (text.contains(mixedCase)) {
            return true;
        }
    }

    return false;
}

void MainWindow::onFileListCustomContextMenuRequested(const QPoint &pos)
{
    QMenu contextMenu(this);
    QAction *translateSelectedAction = contextMenu.addAction("Translate Selected Files");
    QAction *translateAllAction = contextMenu.addAction("Translate All Files");

    connect(translateSelectedAction, &QAction::triggered, this, &MainWindow::onTranslateSelectedFiles);
    connect(translateAllAction, &QAction::triggered, this, &MainWindow::onTranslateAllFiles);

    contextMenu.exec(ui->fileListView->mapToGlobal(pos));
}

void MainWindow::onTranslateSelectedFiles()
{
    QModelIndexList selectedIndexes = ui->fileListView->selectionModel()->selectedIndexes();
    if (selectedIndexes.isEmpty()) {
        QMessageBox::information(this, "Translate", "Please select files to translate.");
        return;
    }

    QStringList availableServices = m_translationServiceManager->getAvailableServices();
    if (availableServices.isEmpty()) {
        QMessageBox::warning(this, "Error", "No translation services available.");
        return;
    }

    bool ok;
    QString serviceName = QInputDialog::getItem(this, "Translate Selected Files",
                                                "Select Translation Service:", availableServices, 0, false, &ok);
    if (!ok || serviceName.isEmpty()) {
        return;
    }

    // Prepare settings once
    QVariantMap settings;
    settings["googleApiKey"] = m_apiKey;
    settings["targetLanguage"] = m_targetLanguage;
    settings["googleApi"] = m_googleApi;
    settings["llmProvider"] = m_llmProvider;
    settings["llmApiKey"] = m_llmApiKey;
    settings["llmModel"] = m_llmModel;
    settings["llmBaseUrl"] = m_llmBaseUrl;

    int queuedFiles = 0;

    for (const QModelIndex &idx : selectedIndexes) {
        QStandardItem *item = m_fileListModel->itemFromIndex(idx);
        if (!item) continue;

        QString filePath = item->data(Qt::UserRole).toString();
        if (!m_projectDataManager->getLoadedGameProjectData().contains(filePath)) continue;

        const QJsonArray &textsArray = m_projectDataManager->getLoadedGameProjectData().value(filePath);
        QStringList sourceTexts;

        for (const QJsonValue &val : textsArray) {
            QJsonObject obj = val.toObject();
            QString source = obj["source"].toString();
            QString translation = obj["text"].toString();

            // Only translate if translation is empty and not code
            if (translation.isEmpty() && !source.isEmpty() && !isLikelyCode(source)) {
                sourceTexts.append(source);
            }
        }

        if (!sourceTexts.isEmpty()) {
            TranslationJob job;
            job.serviceName = serviceName;
            job.sourceTexts = sourceTexts;
            job.settings = settings;
            job.fileIndex = idx;

            // Mark as queued
            QString originalText = item->data(Qt::UserRole + 1).toString();
            if (originalText.isEmpty()) {
                item->setData(item->text(), Qt::UserRole + 1);
            }
            item->setText("⏳ " + item->data(Qt::UserRole + 1).toString());

            m_translationQueue.enqueue(job);
            queuedFiles++;
        }
    }

    if (queuedFiles > 0) {
        processNextTranslationJob();
        statusBar()->showMessage(QString("Queued %1 files for translation.").arg(queuedFiles), 4000);
    } else {
        QMessageBox::information(this, "Translate", "No translatable text found in selected files (or all already translated).");
    }
}

void MainWindow::onTranslateAllFiles()
{
    ui->fileListView->selectAll();
    onTranslateSelectedFiles();
}
