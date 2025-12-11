#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "plugindebuggerdialog.h"
#include "pluginmanagerdialog.h"
#include "loadprojectdialog.h"

#ifdef HAS_LUA
#include "plugins/LuaScriptManager.h"
#endif

#include <QStandardPaths>
#include <QStyle>
#include <QApplication>
#include <QScreen>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QString logFilePath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/mainwindow_log.txt";
    QFile logFile(logFilePath);
    ui->setupUi(this);
    setWindowIcon(QIcon(":/icons/icon-app.png"));
    resize(1024, 768);
    
    // Remove the default menu bar created by ui->setupUi
    if (ui->menubar) {
        delete ui->menubar;
        ui->menubar = nullptr;
    }

    // Initialize Managers
    m_translationServiceManager = new TranslationServiceManager(this);
    connect(m_translationServiceManager, &TranslationServiceManager::errorOccurred, this, [this](const QString &message){
       statusBar()->showMessage("Translation Service Error: " + message, 5000); 
    });

    loadSettings();

    // Customize Window Flags
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    
    // Create Custom Title Bar
    m_titleBar = new CustomTitleBar(this);
    m_titleBar->setTitle("NST Translation Tool");
    m_titleBar->setIcon(QIcon(":/icons/icon-app.png"));
    
    connect(m_titleBar, &CustomTitleBar::minimizeClicked, this, &QMainWindow::showMinimized);
    connect(m_titleBar, &CustomTitleBar::maximizeRestoreClicked, this, [this]() {
        if (isMaximized()) {
            showNormal();
        } else {
            showMaximized();
        }
    });
    connect(m_titleBar, &CustomTitleBar::closeClicked, this, &QMainWindow::close);

    // Setup MenuBar
    m_menuBar = new MenuBar(this);

    // Create Stacked Widget and Pages
    m_stackedWidget = new QStackedWidget(this);
    
    // Page 0: File Translation Widget
    m_fileTranslationWidget = new FileTranslationWidget(m_translationServiceManager, this);
    m_fileTranslationWidget->setSettings(m_apiKey, m_targetLanguage, m_googleApi, 
                                         m_llmProvider, m_llmApiKey, m_llmModel, m_llmBaseUrl);
    m_stackedWidget->addWidget(m_fileTranslationWidget); 
    
    // Page 1: Real-time Translation Widget
    m_realTimeWidget = new RealTimeTranslationWidget(this);
    m_stackedWidget->addWidget(m_realTimeWidget);
    
    // Page 2: Relationship Widget
    m_relationshipWidget = new RelationshipWidget(this);
    m_stackedWidget->addWidget(m_relationshipWidget);
    
    // Create a new container widget
    QWidget *mainContainer = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(mainContainer);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(0);
    
    // Add widgets to layout
    m_titleBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_menuBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_stackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    mainLayout->addWidget(m_titleBar);
    mainLayout->addWidget(m_menuBar);
    mainLayout->addWidget(m_stackedWidget);
    
    mainLayout->setStretch(0, 0); // Title Bar
    mainLayout->setStretch(1, 0); // Menu Bar
    mainLayout->setStretch(2, 1); // Content
    
    setCentralWidget(mainContainer);
    
    // Connect Navigation
    connect(m_titleBar, &CustomTitleBar::translateModeClicked, this, [this]() {
        onNavigationChanged(0);
    });
    connect(m_titleBar, &CustomTitleBar::realTimeModeClicked, this, [this]() {
        onNavigationChanged(1);
    });
    connect(m_titleBar, &CustomTitleBar::relationsModeClicked, this, [this]() {
        onNavigationChanged(2);
    });
    
    // Enable mouse tracking for resizing and moving
    setMouseTracking(true);
    if (centralWidget()) {
        centralWidget()->setMouseTracking(true);
    }
    
    // Set minimum size
    setMinimumSize(800, 600);

    // Connect MenuBar signals to FileTranslationWidget slots
    connect(m_menuBar, &MenuBar::openMockData, m_fileTranslationWidget, &FileTranslationWidget::openMockData);
    connect(m_menuBar, &MenuBar::newProject, this, &MainWindow::onNewProject);
    connect(m_menuBar, &MenuBar::openProject, m_fileTranslationWidget, &FileTranslationWidget::onOpenProject);
    connect(m_fileTranslationWidget, &FileTranslationWidget::projectLoaded, m_relationshipWidget, &RelationshipWidget::loadRelations);
    
    // Changing strategy: Connect to FileTranslationWidget signal if it exists, or add one.
    // Let's assume for now I need to check FileTranslationWidget first.
    connect(m_menuBar, &MenuBar::settings, this, &MainWindow::onSettingsActionTriggered);
    connect(m_menuBar, &MenuBar::saveProject, this, &MainWindow::onSaveProject);
    connect(m_menuBar, &MenuBar::deployProject, this, &MainWindow::onDeployProject);
    connect(m_menuBar, &MenuBar::exit, this, &QMainWindow::close);
    connect(m_menuBar, &MenuBar::fontManager, this, &MainWindow::onFontManagerActionTriggered);
    connect(m_menuBar, &MenuBar::pluginManager, this, &MainWindow::onPluginManagerActionTriggered);
    connect(m_menuBar, &MenuBar::toggleContext, this, &MainWindow::onToggleContext);
    connect(m_menuBar, &MenuBar::hideCompleted, this, &MainWindow::onHideCompleted);
    connect(m_menuBar, &MenuBar::exportSmartFilterRules, this, &MainWindow::onExportSmartFilterRules);
    connect(m_menuBar, &MenuBar::importSmartFilterRules, this, &MainWindow::onImportSmartFilterRules);

    m_updateController = new UpdateController(this);
    m_updateController->checkForUpdates();

#ifdef HAS_LUA
    // Load enabled Lua plugins only
    QString scriptPath = QCoreApplication::applicationDirPath() + "/scripts";
    QDir scriptDir(scriptPath);
    QSettings settings;
    
    int loadedCount = 0;
    for (const QString& file : scriptDir.entryList({"*.lua"}, QDir::Files)) {
        bool enabled = settings.value("plugins/" + file + "/enabled", false).toBool();
        if (enabled) {
            LuaScriptManager::instance().loadScriptsFromDir(scriptPath);
            LuaScriptManager::instance().registerAPI();
            loadedCount++;
        }
    }
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onNewProject()
{
    BGADataManager tempManager(this); 
    QStringList availableEngines = tempManager.getAvailableAnalyzers();
    
    if (availableEngines.isEmpty()) {
        QMessageBox::warning(this, "Error", "No game analyzers available.");
        return;
    }

    LoadProjectDialog dialog(availableEngines, this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QString engineName = dialog.selectedEngine();
    // Update member
    m_engineName = engineName;
    QString projectPath = dialog.projectPath();

    if (engineName.isEmpty() || projectPath.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "Please select both engine and project path.");
        return;
    }
    
    m_fileTranslationWidget->onNewProject(engineName, projectPath);
    
    // Check Engine Capability for Relations
    m_engineName = engineName; 
    
    bool isRpgMaker = (engineName.startsWith("RPG Maker"));
    bool showRelations = m_enableRelations && isRpgMaker;
    m_titleBar->setRelationsVisible(showRelations);
    
    // Show file translation widget
    m_stackedWidget->setCurrentWidget(m_fileTranslationWidget);
}

void MainWindow::onOpenMockData()
{
    m_fileTranslationWidget->openMockData();
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
    dialog.setLlmModel(m_llmModel);
    dialog.setLlmBaseUrl(m_llmBaseUrl);
    dialog.setRelationsEnabled(m_enableRelations);

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
        m_llmApiKey = dialog.llmApiKey();
        m_llmModel = dialog.llmModel();
        m_llmBaseUrl = dialog.llmBaseUrl();
        m_enableRelations = dialog.isRelationsEnabled();
        saveSettings();
        updateChildSettings();
        
        // Update TitleBar Visibility immediately
        // We also need to check current engine if project is loaded
        bool isRpgMaker = (m_engineName.startsWith("RPG Maker"));
        bool showRelations = m_enableRelations && isRpgMaker;
        m_titleBar->setRelationsVisible(showRelations);
    }
}

void MainWindow::loadSettings()
{
     QSettings settings;
     m_apiKey = settings.value("googleApiKey").toString();
     m_targetLanguage = settings.value("targetLanguage", "th").toString();
     m_googleApi = settings.value("googleApi", false).toBool();
     m_llmProvider = settings.value("llmProvider").toString();
     m_llmApiKey = settings.value("llmApiKey").toString();
     m_llmApiKey = settings.value("llmApiKey").toString();
     m_llmModel = settings.value("llmModel").toString();
     m_llmBaseUrl = settings.value("llmBaseUrl").toString();
     m_enableRelations = settings.value("enableRelations", true).toBool(); // Default true
}

void MainWindow::saveSettings()
{
     QSettings settings;
     settings.setValue("googleApiKey", m_apiKey);
     settings.setValue("enableRelations", m_enableRelations);
     settings.setValue("targetLanguage", m_targetLanguage);
     settings.setValue("googleApi", m_googleApi);
     settings.setValue("llmProvider", m_llmProvider);
     settings.setValue("llmApiKey", m_llmApiKey);
     settings.setValue("llmModel", m_llmModel);
     settings.setValue("llmBaseUrl", m_llmBaseUrl);
}

void MainWindow::updateChildSettings()
{
    if (m_fileTranslationWidget) {
        m_fileTranslationWidget->setSettings(m_apiKey, m_targetLanguage, m_googleApi,
                                             m_llmProvider, m_llmApiKey, m_llmModel, m_llmBaseUrl);
    }
}

void MainWindow::onFontsLoaded(const QJsonArray &fonts) {}

void MainWindow::onFontManagerActionTriggered()
{
    m_fileTranslationWidget->openFontManager();
}


void MainWindow::onPluginManagerActionTriggered()
{
    PluginManagerDialog dialog(this);
    dialog.exec();
}

void MainWindow::onToggleContext(bool checked) { m_fileTranslationWidget->onToggleContext(checked); }
void MainWindow::onHideCompleted(bool checked) { m_fileTranslationWidget->onHideCompleted(checked); }
void MainWindow::onExportSmartFilterRules() { m_fileTranslationWidget->onExportSmartFilterRules(); }
void MainWindow::onImportSmartFilterRules() { m_fileTranslationWidget->onImportSmartFilterRules(); }
void MainWindow::onSaveProject() { m_fileTranslationWidget->onSaveProject(); }
void MainWindow::onDeployProject() { m_fileTranslationWidget->onDeployProject(); }

void MainWindow::onNavigationChanged(int index)
{
    m_stackedWidget->setCurrentIndex(index);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_resizeDirection = getResizeDirection(event->pos());
        if (m_resizeDirection != ResizeNone) {
            m_isDragging = true;
            m_dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
            m_originalGeometry = geometry();
        } else {
             // If we prefer CustomTitleBar to handle moving, we rely on its signals or this logic if it bubbles up.
             m_isDragging = true;
             m_dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
             m_resizeDirection = ResizeNone; 
        }
    }
    QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
        m_resizeDirection = ResizeNone;
        unsetCursor();
    }
    QMainWindow::mouseReleaseEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    updateCursorShape(event->pos());

    if (m_isDragging) {
        if (m_resizeDirection != ResizeNone) {
            QPoint globalPos = event->globalPosition().toPoint();
            QRect geom = m_originalGeometry;
            
            int left = geom.left();
            int top = geom.top();
            int right = geom.right();
            int bottom = geom.bottom();
            
            // Note: globalPos is screen coordinate
            if (m_resizeDirection & ResizeLeft)   left = globalPos.x();
            if (m_resizeDirection & ResizeRight)  right = globalPos.x();
            if (m_resizeDirection & ResizeTop)    top = globalPos.y();
            if (m_resizeDirection & ResizeBottom) bottom = globalPos.y();
            
            QRect newGeom(QPoint(left, top), QPoint(right, bottom));
            
            // Enforce minimum size
            if (newGeom.width() < minimumWidth()) {
                 if (m_resizeDirection & ResizeLeft) newGeom.setLeft(right - minimumWidth());
                 else newGeom.setRight(left + minimumWidth());
            }
            if (newGeom.height() < minimumHeight()) {
                 if (m_resizeDirection & ResizeTop) newGeom.setTop(bottom - minimumHeight());
                 else newGeom.setBottom(top + minimumHeight());
            }

            setGeometry(newGeom);
        } else {
             move(event->globalPosition().toPoint() - m_dragPosition);
        }
    }
    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::updateCursorShape(const QPoint &pos)
{
    if (m_isDragging) return; 

    int dir = getResizeDirection(pos);
    switch (dir) {
        case ResizeTopLeft:
        case ResizeBottomRight:
            setCursor(Qt::SizeFDiagCursor);
            break;
        case ResizeTopRight:
        case ResizeBottomLeft:
            setCursor(Qt::SizeBDiagCursor);
            break;
        case ResizeLeft:
        case ResizeRight:
            setCursor(Qt::SizeHorCursor);
            break;
        case ResizeTop:
        case ResizeBottom:
            setCursor(Qt::SizeVerCursor);
            break;
        default:
            unsetCursor();
            break;
    }
}

int MainWindow::getResizeDirection(const QPoint &pos)
{
    int dir = ResizeNone;
    const int margin = 5; // Resize margin

    int w = width();
    int h = height();
    int x = pos.x();
    int y = pos.y();

    if (x < margin) dir |= ResizeLeft;
    if (x > w - margin) dir |= ResizeRight;
    if (y < margin) dir |= ResizeTop;
    if (y > h - margin) dir |= ResizeBottom;

    return dir;
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (m_titleBar && m_titleBar->geometry().contains(event->pos())) {
         if (isMaximized()) showNormal();
         else showMaximized();
    }
    QMainWindow::mouseDoubleClickEvent(event);
}
