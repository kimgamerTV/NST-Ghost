#pragma once
#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QCheckBox>

class PluginManagerDialog : public QDialog {
    Q_OBJECT
public:
    explicit PluginManagerDialog(QWidget* parent = nullptr);

private slots:
    void onPluginSelected();
    void onInstallClicked();
    void onRunActionClicked();
    void onReloadClicked();
    void onEnableToggled(int state);

private:
    QListWidget* m_pluginList;
    QTextEdit* m_logOutput;
    QTextEdit* m_infoText;
    QPushButton* m_installBtn;
    QPushButton* m_runActionBtn;
    QCheckBox* m_enableCheckBox;
    
    void loadPlugins();
    void appendLog(const QString& msg);
    QString getPluginStatus(const QString& pluginName);
};
