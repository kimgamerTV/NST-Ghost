#ifndef MENUBAR_H
#define MENUBAR_H

#include <QMenuBar>

class MenuBar : public QMenuBar
{
    Q_OBJECT
public:
    explicit MenuBar(QWidget *parent = nullptr);

signals:
    void openMockData();
    void loadFromGameProject();
    void settings();
    void saveProject(); // New signal for saving project
    void exit();
    void fontManager();
    void pluginManager();

    // View signals
    void toggleContext(bool checked);
    void hideCompleted(bool checked);

    // Smart Filter signals
    void exportSmartFilterRules();
    void importSmartFilterRules();

private:
    void createMenus();
    void createActions();

    QMenu *fileMenu;
    QMenu *viewMenu; // New View menu
    QMenu *toolsMenu;
    QMenu *smartFilterMenu; // New menu
    QAction *openMockDataAction;
    QAction *loadFromGameProjectAction;
    QAction *settingsAction;
    QAction *saveProjectAction; // New action declaration
    QAction *exitAction;
    QAction *fontManagerAction;
    QAction *pluginManagerAction;

    // View actions
    QAction *toggleContextAction;
    QAction *hideCompletedAction;

    // Smart Filter actions
    QAction *exportSmartFilterRulesAction;
    QAction *importSmartFilterRulesAction;
};

#endif // MENUBAR_H