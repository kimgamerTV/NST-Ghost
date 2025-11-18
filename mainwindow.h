#include "customprogressdialog.h"
#include <QMainWindow>
#include <QStandardItemModel>
#include <QStringListModel>
#include <QMap>
#include <QJsonArray>
#include <QJsonObject>
#include <QFutureWatcher>
#include <QProgressDialog>

#include "searchcontroller.h"
#include "searchdialog.h"
#include "shortcutcontroller.h"
#include "bgadatamanager.h"
#include "translationservicemanager.h"
#include "menubar.h"
#include "settingsdialog.h"
#include "fontmanagerdialog.h"
#include "updatecontroller.h"
#include "translationcontextmenu.h"
#include "projectdatamanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void onLoadFromGameProject();

private slots:
    void onLoadingFinished();
    void onProjectProcessingFinished();
    void openSearchDialog();
    void onSearchResultSelected(const QString &fileName, int row);
    void onOpenMockData();
    void onBGADataError(const QString &message);
    void onSearchRequested(const QString &query);
    void onTranslationFinished(const qtlingo::TranslationResult &result);
    void onTranslationServiceError(const QString &message);
    void onTranslationTableViewCustomContextMenuRequested(const QPoint &pos);
    void onTranslateSelectedTextWithService();
    void onTranslateAllSelectedText();
    void onSelectAllRequested();
    void onSettingsActionTriggered();
    void onFontsLoaded(const QJsonArray &fonts);
    void onFontManagerActionTriggered();
    void onPluginManagerActionTriggered();
    void onMockDataLoaded(const QJsonArray &data);
    void onUndoTranslation();
    void onSaveGameProject();
    void onTranslationDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

private:
    void loadSettings();
    void saveSettings();

private:
    Ui::MainWindow *ui;
    QStringListModel *m_fileListModel;
    QStandardItemModel *m_translationModel;
    SearchController *m_searchController;
    SearchDialog *m_searchDialog;
    ShortcutController *m_shortcutController;
    BGADataManager *m_bgaDataManager;
    TranslationServiceManager *m_translationServiceManager;
    MenuBar *m_menuBar;
    UpdateController *m_updateController;
    ProjectDataManager *m_projectDataManager;

    QString m_apiKey;
    QString m_targetLanguage;
    QString m_targetLanguageName;
    bool m_googleApi;
    QString m_llmProvider;
    QString m_llmApiKey;
    QString m_llmModel;
    QString m_llmBaseUrl;

    QMultiMap<QString, QModelIndex> m_pendingTranslations;

    QJsonArray m_gameFonts;

    QString m_currentEngineName;
    QString m_currentProjectPath;

    QFutureWatcher<QJsonArray> m_loadFutureWatcher;
    CustomProgressDialog *m_progressDialog;
};

