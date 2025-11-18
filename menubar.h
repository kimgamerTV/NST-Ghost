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

private:
    void createMenus();
    void createActions();

    QMenu *fileMenu;
    QMenu *toolsMenu;
    QAction *openMockDataAction;
    QAction *loadFromGameProjectAction;
    QAction *settingsAction;
    QAction *saveProjectAction; // New action declaration
    QAction *exitAction;
    QAction *fontManagerAction;
    QAction *pluginManagerAction;
};

#endif // MENUBAR_H