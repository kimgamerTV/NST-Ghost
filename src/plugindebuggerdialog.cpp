#include "plugindebuggerdialog.h"
#include <QDir>
#include <QCoreApplication>
#include <QDateTime>

PluginDebuggerDialog::PluginDebuggerDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Plugin Debugger");
    resize(800, 600);

    auto *mainLayout = new QVBoxLayout(this);

    // Script Selection
    auto *topLayout = new QHBoxLayout();
    topLayout->addWidget(new QLabel("Select Script:"));
    m_scriptCombo = new QComboBox();
    topLayout->addWidget(m_scriptCombo);
    m_testButton = new QPushButton("Test Translate");
    topLayout->addWidget(m_testButton);
    mainLayout->addLayout(topLayout);

    // Input
    mainLayout->addWidget(new QLabel("Input Text:"));
    m_inputEdit = new QTextEdit();
    m_inputEdit->setPlaceholderText("Enter text to translate...");
    m_inputEdit->setMaximumHeight(100);
    mainLayout->addWidget(m_inputEdit);

    // Output
    mainLayout->addWidget(new QLabel("Output Result:"));
    m_outputEdit = new QTextEdit();
    m_outputEdit->setReadOnly(true);
    m_outputEdit->setMaximumHeight(100);
    mainLayout->addWidget(m_outputEdit);

    // Logs
    mainLayout->addWidget(new QLabel("Logs:"));
    m_logEdit = new QTextEdit();
    m_logEdit->setReadOnly(true);
    m_logEdit->setStyleSheet("background-color: #2b2b2b; color: #cccccc; font-family: monospace;");
    mainLayout->addWidget(m_logEdit);

    connect(m_testButton, &QPushButton::clicked, this, &PluginDebuggerDialog::onTestClicked);

    loadScripts();
}

PluginDebuggerDialog::~PluginDebuggerDialog()
{
    if (m_currentService) {
        delete m_currentService;
    }
}

void PluginDebuggerDialog::loadScripts()
{
    QDir scriptDir(QCoreApplication::applicationDirPath() + "/scripts");
    QStringList scripts = scriptDir.entryList({"*.lua"}, QDir::Files);
    m_scriptCombo->addItems(scripts);
}

void PluginDebuggerDialog::onTestClicked()
{
    QString scriptName = m_scriptCombo->currentText();
    QString inputText = m_inputEdit->toPlainText();

    if (scriptName.isEmpty() || inputText.isEmpty()) return;

    m_outputEdit->clear();
    m_logEdit->clear();
    m_logEdit->append("--- Starting Test ---");

    if (m_currentService) {
        delete m_currentService;
        m_currentService = nullptr;
    }

    QString scriptPath = QCoreApplication::applicationDirPath() + "/scripts/" + scriptName;
    m_currentService = new LuaTranslationService(scriptPath, this);

    connect(m_currentService, &LuaTranslationService::translationFinished, this, &PluginDebuggerDialog::onTranslationFinished);
    connect(m_currentService, &LuaTranslationService::errorOccurred, this, &PluginDebuggerDialog::onErrorOccurred);
    connect(m_currentService, &LuaTranslationService::logMessage, this, &PluginDebuggerDialog::onLogMessage);

    m_currentService->translate(inputText);
}

void PluginDebuggerDialog::onTranslationFinished(const qtlingo::TranslationResult &result)
{
    m_outputEdit->setText(result.translatedText);
    m_logEdit->append("--- Finished Successfully ---");
}

void PluginDebuggerDialog::onErrorOccurred(const QString &message)
{
    m_outputEdit->setText("ERROR: " + message);
    m_logEdit->append("--- Error Occurred ---");
    m_logEdit->append(message);
}

void PluginDebuggerDialog::onLogMessage(const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");
    m_logEdit->append(QString("[%1] %2").arg(timestamp, message));
}
