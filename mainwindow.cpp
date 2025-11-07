#include "mainwindow.h"
#include "./ui_mainwindow.h"

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

#include "customprogressdialog.h"
#include "loadprojectdialog.h"
#include "projectdatamanager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_progressDialog(nullptr)
{
    QString logFilePath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/mainwindow_log.txt";
    QFile logFile(logFilePath);
    ui->setupUi(this);
    setWindowIcon(QIcon(":/icons/icon-app.png"));
    resize(1024, 768); // Set a reasonable default size

    m_fileListModel = new QStringListModel(this);
    ui->fileListView->setModel(m_fileListModel);

    // Set icon provider
    QFileIconProvider *iconProvider = new QFileIconProvider();
    ui->fileListView->setIconSize(QSize(24, 24));

    m_translationModel = new QStandardItemModel(this);
    ui->translationTableView->setModel(m_translationModel);

    // ===== ปรับการแสดงผลตาราง =====
    ui->translationTableView->setWordWrap(true);
    ui->translationTableView->setTextElideMode(Qt::ElideNone);
    ui->translationTableView->setAlternatingRowColors(true);

    // ปรับ Vertical Header
    ui->translationTableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
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

    m_menuBar = new MenuBar(this);
    setMenuBar(m_menuBar);

    connect(m_menuBar, &MenuBar::openMockData, this, &MainWindow::onOpenMockData);
    connect(m_menuBar, &MenuBar::loadFromGameProject, this, &MainWindow::onLoadFromGameProject);
    connect(m_menuBar, &MenuBar::settings, this, &MainWindow::onSettingsActionTriggered);
    connect(m_menuBar, &MenuBar::saveProject, this, &MainWindow::onSaveGameProject);
    connect(m_menuBar, &MenuBar::exit, this, &QMainWindow::close);
    connect(m_menuBar, &MenuBar::fontManager, this, &MainWindow::onFontManagerActionTriggered);

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

    if (!m_fileListModel->stringList().isEmpty()) {
        // Auto-select first file
        QModelIndex firstIndex = m_fileListModel->index(0, 0);
        ui->fileListView->setCurrentIndex(firstIndex);
        // Call onFileSelected directly on ProjectDataManager
        m_projectDataManager->onFileSelected(firstIndex);
    }
}

MainWindow::~MainWindow()
{
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
    m_fileListModel->setStringList(QStringList());

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
    int idx = m_fileListModel->stringList().indexOf(fileName);
    if (idx >= 0) {
        QModelIndex modelIdx = m_fileListModel->index(idx, 0);
        ui->fileListView->setCurrentIndex(modelIdx);
        ui->translationTableView->setUpdatesEnabled(false);
        m_projectDataManager->onFileSelected(modelIdx);
        ui->translationTableView->setUpdatesEnabled(true);
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
    m_fileListModel->setStringList(QStringList());

    QMap<QString, QJsonArray> fileMap;
    fileMap["script1.json"] = mockArray;
    m_projectDataManager->getLoadedGameProjectData() = fileMap;
    m_fileListModel->setStringList(QStringList() << "script1.json");

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
    const QString &sourceText = result.sourceText;
    const QString &translatedText = result.translatedText;

    auto indexes = m_pendingTranslations.values(sourceText);
    for (const QModelIndex &idx : indexes) {
        if (idx.isValid()) {
            m_translationModel->setData(idx, translatedText);
        }
    }
    m_pendingTranslations.remove(sourceText);
}

void MainWindow::onTranslationServiceError(const QString &message)
{
    QMessageBox::warning(this, "Translation Error", message);
    m_pendingTranslations.clear();
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
    for (const QModelIndex &selectedIndex : selectedIndexes) {
        if (selectedIndex.column() != 0) continue;
        QString sourceText = m_translationModel->data(selectedIndex, Qt::DisplayRole).toString();
        if (!sourceText.isEmpty()) {
            sourceTexts.append(sourceText);
            m_pendingTranslations.insert(sourceText, m_translationModel->index(selectedIndex.row(), 1));
        }
    }

    if (!sourceTexts.isEmpty()) {
        QVariantMap settings;
        settings["googleApiKey"] = m_apiKey;
        settings["targetLanguage"] = m_targetLanguage;
        settings["googleApi"] = m_googleApi;
        settings["llmProvider"] = m_llmProvider;
        settings["llmApiKey"] = m_llmApiKey;
        settings["llmModel"] = m_llmModel;
        settings["llmBaseUrl"] = m_llmBaseUrl;

        m_translationServiceManager->translate(serviceName, sourceTexts, settings);
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
