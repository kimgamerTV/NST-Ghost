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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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

    // Setup search controller
    m_searchController = new SearchController(m_translationModel, ui->translationTableView, this);
    m_searchController->setTranslationModel(m_translationModel);
    m_searchController->setLoadedGameProjectData(&m_loadedGameProjectData);
    m_searchController->setFileListModel(m_fileListModel);

    // Setup search dialog
    m_searchDialog = new SearchDialog(this);
    connect(m_searchDialog, &SearchDialog::searchRequested, this, &MainWindow::onSearchRequested);
    connect(m_searchDialog, &SearchDialog::resultSelected, this, &MainWindow::onSearchResultSelected);

    // Setup shortcut controller
    m_shortcutController = new ShortcutController(this);
    m_shortcutController->createShortcuts();
    connect(m_shortcutController, &ShortcutController::focusSearch, this, &MainWindow::openSearchDialog);
    m_shortcutController->createSelectAllShortcut(ui->translationTableView);
    connect(m_shortcutController, &ShortcutController::selectAllRequested, this, &MainWindow::onSelectAllRequested);

    // Setup BGA Data Manager
    m_bgaDataManager = new BGADataManager(this);
    connect(m_bgaDataManager, &BGADataManager::errorOccurred, this, &MainWindow::onBGADataError);

    // Setup Translation Service Manager
    m_translationServiceManager = new TranslationServiceManager(this);
    connect(m_translationServiceManager, &TranslationServiceManager::translationFinished, this, &MainWindow::onTranslationFinished);
    connect(m_translationServiceManager, &TranslationServiceManager::errorOccurred, this, &MainWindow::onTranslationServiceError);

    // Connect menu actions
    connect(ui->actionOpen_Mock_Data, &QAction::triggered, this, &MainWindow::onOpenMockData);
    connect(ui->actionLoad_from_Game_Project, &QAction::triggered, this, &MainWindow::onLoadFromGameProject);
    connect(ui->actionSettings, &QAction::triggered, this, &MainWindow::onSettingsActionTriggered);
    connect(ui->action_Exit, &QAction::triggered, this, &QMainWindow::close);

    // Enable custom context menu for translation table view
    ui->translationTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->translationTableView, &QTableView::customContextMenuRequested, this, &MainWindow::onTranslationTableViewCustomContextMenuRequested);

    loadSettings();

    // Populate file list with mock data initially
    onOpenMockData();

    m_updateController = new UpdateController(this);
    m_updateController->checkForUpdates();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_fileListView_clicked(const QModelIndex &index)
{
    QString fileName = m_fileListModel->data(index, Qt::DisplayRole).toString();

    m_translationModel->clear();
    m_translationModel->setHorizontalHeaderLabels(QStringList() << "Source Text" << "Translation");

    // Find the full file path from the display name
    QString fullFilePath = fileName;
    for (const QString &key : m_loadedGameProjectData.keys()) {
        if (QFileInfo(key).fileName() == fileName) {
            fullFilePath = key;
            break;
        }
    }

    if (m_loadedGameProjectData.contains(fullFilePath)) {
        QJsonArray textsArray = m_loadedGameProjectData.value(fullFilePath);
        for (const QJsonValue &value : textsArray) {
            if (value.isObject()) {
                QJsonObject textObject = value.toObject();
                QString text = textObject["text"].toString();

                if (!text.isEmpty()) {
                    QList<QStandardItem *> rowItems;

                    // สร้าง Source Text Item
                    QStandardItem *sourceItem = new QStandardItem(text);
                    sourceItem->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
                    sourceItem->setFlags(sourceItem->flags() & ~Qt::ItemIsEditable); // ไม่ให้แก้ไข Source

                    // เพิ่ม padding ด้วย margin (ใช้ data role สำหรับ custom delegate ถ้ามี)
                    QFont font = sourceItem->font();
                    sourceItem->setFont(font);

                    // สร้าง Translation Item
                    QStandardItem *translationItem = new QStandardItem("");
                    translationItem->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
                    translationItem->setFlags(translationItem->flags() | Qt::ItemIsEditable); // ให้แก้ไขได้
                    translationItem->setFont(font);

                    rowItems << sourceItem << translationItem;
                    m_translationModel->appendRow(rowItems);
                }
            }
        }
    } else {
        QList<QStandardItem *> rowItems;
        QStandardItem *emptyItem = new QStandardItem("No strings found for this file.");
        emptyItem->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
        emptyItem->setFlags(emptyItem->flags() & ~Qt::ItemIsEditable);
        rowItems << emptyItem << new QStandardItem("");
        m_translationModel->appendRow(rowItems);
    }

    // ปรับขนาดแถวให้พอดีกับเนื้อหา
    ui->translationTableView->resizeRowsToContents();

    // รีเซ็ตความกว้างคอลัมน์ Source Text
    ui->translationTableView->setColumnWidth(0, 400);
}

void MainWindow::openSearchDialog()
{
    m_searchDialog->show();
    m_searchDialog->raise();
    m_searchDialog->activateWindow();
}

void MainWindow::onSearchResultSelected(const QString &fileName, int row)
{
    // Find the file in the file list view and select it
    QModelIndexList indexes = m_fileListModel->match(m_fileListModel->index(0, 0), Qt::DisplayRole, QFileInfo(fileName).fileName(), 1, Qt::MatchExactly);
    if (!indexes.isEmpty()) {
        ui->fileListView->setCurrentIndex(indexes.first());
        on_fileListView_clicked(indexes.first()); // Load the file data

        // Scroll to the selected row in the translation table view
        ui->translationTableView->scrollTo(m_translationModel->index(row, 0));
        ui->translationTableView->selectRow(row);
    }
}

void MainWindow::onOpenMockData()
{
    m_loadedGameProjectData.clear(); // Clear any previously loaded data

    // Populate m_loadedGameProjectData with mock data
    QJsonArray commonData;
    QJsonObject common1, common2, common3;
    common1["key"] = "Hello"; common1["text"] = "Hello"; common1["file"] = "Common.ts";
    common2["key"] = "LongSentence"; common2["text"] = "This is a very long sentence to test the word wrapping feature in the table view. I hope it works as expected."; common2["file"] = "Common.ts";
    common3["key"] = "Save"; common3["text"] = "Save"; common3["file"] = "Common.ts";
    commonData.append(common1); commonData.append(common2); commonData.append(common3);
    m_loadedGameProjectData["Common.ts"] = commonData;

    QJsonArray settingsData;
    QJsonObject settings1, settings2, settings3;
    settings1["key"] = "Language"; settings1["text"] = "Language"; settings1["file"] = "Settings.ts";
    settings2["key"] = "Appearance"; settings2["text"] = "Appearance"; settings2["file"] = "Settings.ts";
    settings3["key"] = "DarkMode"; settings3["text"] = "Enable Dark Mode. This will restart the application."; settings3["file"] = "Settings.ts";
    settingsData.append(settings1); settingsData.append(settings2); settingsData.append(settings3);
    m_loadedGameProjectData["Settings.ts"] = settingsData;

    // Populate file list with keys from m_loadedGameProjectData
    QStringList fileNamesForDisplay;
    for (const QString &key : m_loadedGameProjectData.keys()) {
        fileNamesForDisplay.append(QFileInfo(key).fileName());
    }
    m_fileListModel->setStringList(fileNamesForDisplay);

    if (m_fileListModel->rowCount() > 0) {
        ui->fileListView->setCurrentIndex(m_fileListModel->index(0, 0));
        on_fileListView_clicked(m_fileListModel->index(0, 0));
    }
}

void MainWindow::onLoadFromGameProject()
{
    QStringList availableAnalyzers = m_bgaDataManager->getAvailableAnalyzers();
    if (availableAnalyzers.isEmpty()) {
        QMessageBox::warning(this, "Error", "No game engine analyzers available. Please ensure BGACore is correctly built and linked.");
        return;
    }

    LoadProjectDialog dialog(availableAnalyzers, this);
    if (dialog.exec() == QDialog::Accepted) {
        QString engineName = dialog.selectedEngine();
        QString projectPath = dialog.projectPath();

        if (projectPath.isEmpty()) {
            QMessageBox::warning(this, "Error", "Project path cannot be empty.");
            return;
        }

        QJsonArray extractedTextsArray = m_bgaDataManager->loadStringsFromGameProject(engineName, projectPath);

        if (!extractedTextsArray.isEmpty()) {
            m_loadedGameProjectData.clear(); // Clear previous data
            QStringList fileNamesForDisplay;

            for (const QJsonValue &value : extractedTextsArray) {
                if (value.isObject()) {
                    QJsonObject textObject = value.toObject();
                    QString filePath = textObject["file"].toString();
                    if (!filePath.isEmpty()) {
                        // Store texts grouped by full file path
                        m_loadedGameProjectData[filePath].append(value);
                        if (!fileNamesForDisplay.contains(QFileInfo(filePath).fileName())) {
                            fileNamesForDisplay.append(QFileInfo(filePath).fileName());
                        }
                    }
                }
            }
            m_fileListModel->setStringList(fileNamesForDisplay);

            if (m_fileListModel->rowCount() > 0) {
                ui->fileListView->setCurrentIndex(m_fileListModel->index(0, 0));
                on_fileListView_clicked(m_fileListModel->index(0, 0));
            }

        } else {
            QMessageBox::information(this, "Info", "No translatable strings found or extracted.");
        }
    }
}

void MainWindow::onBGADataError(const QString &message)
{
    QMessageBox::critical(this, "BGA Data Error", message);
}

void MainWindow::onSearchRequested(const QString &query)
{
    QList<QPair<QString, QPair<int, QString>>> results;
    // Always search the currently loaded data (mock or BGA)
    results = m_searchController->searchAllFiles(query);
    m_searchDialog->displaySearchResults(results, query);
}

void MainWindow::onTranslateSelectedTextWithService(const QString &serviceName, const QString &sourceText)
{
    // Store the index for later update
    QModelIndexList matchingIndexes = m_translationModel->match(m_translationModel->index(0, 0),
                                                                Qt::DisplayRole, sourceText, 1, Qt::MatchExactly);
    if (!matchingIndexes.isEmpty()) {
        m_pendingTranslations.insert(sourceText, m_translationModel->index(matchingIndexes.first().row(), 1));

        QVariantMap settings;
        settings["googleApiKey"] = m_apiKey;
        settings["targetLanguage"] = m_targetLanguage;
        settings["googleApi"] = m_googleApi;
        settings["llmProvider"] = m_llmProvider;
        settings["llmApiKey"] = m_llmApiKey;
        settings["llmModel"] = m_llmModel;

        m_translationServiceManager->translate(serviceName, sourceText, settings);
    }
}

void MainWindow::onTranslationFinished(const qtlingo::TranslationResult &result)
{
    // Find all matching entries and update them
    QList<QModelIndex> indexes = m_pendingTranslations.values(result.sourceText);
    for (const QModelIndex &index : indexes) {
        m_translationModel->setData(index, result.translatedText);
    }
    m_pendingTranslations.remove(result.sourceText);

    // ปรับขนาดแถวหลังจากแปล
    ui->translationTableView->resizeRowsToContents();
}

void MainWindow::onTranslationServiceError(const QString &message)
{
    QMessageBox::critical(this, "Translation Service Error", message);
}

void MainWindow::onTranslationTableViewCustomContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = ui->translationTableView->indexAt(pos);
    if (!index.isValid()) {
        return;
    }

    TranslationContextMenu contextMenu(m_translationServiceManager, index, ui->translationTableView->selectionModel(), this);
    connect(&contextMenu, &TranslationContextMenu::translateRequested, this, &MainWindow::onTranslateSelectedTextWithService);
    connect(&contextMenu, &TranslationContextMenu::translateAllSelected, this, &MainWindow::onTranslateAllSelectedText);

    contextMenu.exec(ui->translationTableView->viewport()->mapToGlobal(pos));
}

void MainWindow::onTranslateAllSelectedText()
{
    QModelIndexList selectedIndexes = ui->translationTableView->selectionModel()->selectedRows();

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

    for (const QModelIndex &selectedIndex : selectedIndexes) {
        QModelIndex sourceIndex = m_translationModel->index(selectedIndex.row(), 0);
        QString sourceText = m_translationModel->data(sourceIndex, Qt::DisplayRole).toString();

        if (!sourceText.isEmpty()) {
            // Store the index for later update
            m_pendingTranslations.insert(sourceText, m_translationModel->index(selectedIndex.row(), 1));

            QVariantMap settings;
            settings["googleApiKey"] = m_apiKey;
            settings["targetLanguage"] = m_targetLanguage;
            settings["googleApi"] = m_googleApi;
            settings["llmProvider"] = m_llmProvider;
            settings["llmApiKey"] = m_llmApiKey;
            settings["llmModel"] = m_llmModel;

            m_translationServiceManager->translate(serviceName, sourceText, settings);
        }
    }
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
}
