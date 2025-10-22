#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QStringListModel>
#include <QJsonArray>
#include <QMap>
#include <QPair>

#include "searchcontroller.h"
#include "shortcutcontroller.h"
#include "searchdialog.h"
#include "bgadatamanager.h"
#include "loadprojectdialog.h"
#include "translationservicemanager.h"
#include "translationcontextmenu.h"
#include "settingsdialog.h"
#include "updatecontroller.h"
#include <qtlingo/translationservice.h>

#include "menubar.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void on_fileListView_clicked(const QModelIndex &index);
    void openSearchDialog();
    void onSearchResultSelected(const QString &fileName, int row);
    void onLoadFromGameProject();


    void onOpenMockData();
    void onBGADataError(const QString &message);
    void onSearchRequested(const QString &query);
    void onTranslateSelectedTextWithService(const QString &serviceName, const QString &sourceText);
    void onTranslationFinished(const qtlingo::TranslationResult &result);
    void onTranslationServiceError(const QString &message);
    void onTranslationTableViewCustomContextMenuRequested(const QPoint &pos);
    void onTranslateAllSelectedText();
    void onSelectAllRequested();
    void onSettingsActionTriggered();
    void onUndoTranslation();
    void onSaveGameProject(); // New slot for saving game project // New slot for undoing translation
    void onTranslationDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

private:
    void loadSettings();
    void saveSettings();

private:
    Ui::MainWindow *ui;
    MenuBar *m_menuBar;
    QStringListModel *m_fileListModel;
    QStandardItemModel *m_translationModel;
    SearchController *m_searchController;
    ShortcutController *m_shortcutController;
    SearchDialog *m_searchDialog;
    BGADataManager *m_bgaDataManager;
    QMap<QString, QJsonArray> m_loadedGameProjectData;
    TranslationServiceManager *m_translationServiceManager;
    UpdateController *m_updateController;
    QMultiMap<QString, QModelIndex> m_pendingTranslations;

    QString m_apiKey;
    QString m_targetLanguage;
    QString m_targetLanguageName;
    QString m_currentEngineName; // New member to store the current engine name
    QString m_currentProjectPath; // New member to store the current project path
    QString m_currentLoadedFilePath; // New member to store the currently loaded file path

    // Google Translate settings
    bool m_googleApi;

    // LLM settings
    QString m_llmProvider;
    QString m_llmApiKey;
            QString m_llmModel;
        
        };
        #endif // MAINWINDOW_H
