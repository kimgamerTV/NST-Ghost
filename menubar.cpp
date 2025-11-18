#include "menubar.h"
#include <QMenu>

MenuBar::MenuBar(QWidget *parent)
    : QMenuBar(parent)
{
    createActions();
    createMenus();
}

void MenuBar::createMenus()
{
    fileMenu = addMenu(tr("&File"));
    fileMenu->addAction(openMockDataAction);
    fileMenu->addAction(loadFromGameProjectAction);
    fileMenu->addAction(saveProjectAction); // Add save action
    fileMenu->addSeparator();
    fileMenu->addAction(settingsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    toolsMenu = addMenu(tr("&Tools"));
    toolsMenu->addAction(fontManagerAction);
    toolsMenu->addAction(pluginManagerAction);
}

void MenuBar::createActions()
{
    openMockDataAction = new QAction(tr("Open Mock Data"), this);
    connect(openMockDataAction, &QAction::triggered, this, &MenuBar::openMockData);

    loadFromGameProjectAction = new QAction(tr("Load from Game Project..."), this);
    connect(loadFromGameProjectAction, &QAction::triggered, this, &MenuBar::loadFromGameProject);

    saveProjectAction = new QAction(tr("Save Project"), this); // New save action
    connect(saveProjectAction, &QAction::triggered, this, &MenuBar::saveProject);

    settingsAction = new QAction(tr("Settings..."), this);
    connect(settingsAction, &QAction::triggered, this, &MenuBar::settings);

    exitAction = new QAction(tr("&Exit"), this);
    connect(exitAction, &QAction::triggered, this, &MenuBar::exit);

    fontManagerAction = new QAction(tr("Font Manager..."), this);
    connect(fontManagerAction, &QAction::triggered, this, &MenuBar::fontManager);

    pluginManagerAction = new QAction(tr("Plugin Manager..."), this);
    connect(pluginManagerAction, &QAction::triggered, this, &MenuBar::pluginManager);
}
