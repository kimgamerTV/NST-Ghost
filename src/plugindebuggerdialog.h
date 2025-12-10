#pragma once

#include <QDialog>
#include <QComboBox>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "plugins/LuaTranslationPlugin/luatranslationservice.h"

class PluginDebuggerDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PluginDebuggerDialog(QWidget *parent = nullptr);
    ~PluginDebuggerDialog();

private slots:
    void onTestClicked();
    void onTranslationFinished(const qtlingo::TranslationResult &result);
    void onErrorOccurred(const QString &message);
    void onLogMessage(const QString &message);

private:
    void loadScripts();

    QComboBox *m_scriptCombo;
    QTextEdit *m_inputEdit;
    QTextEdit *m_outputEdit;
    QTextEdit *m_logEdit;
    QPushButton *m_testButton;
    
    LuaTranslationService *m_currentService = nullptr;
};
