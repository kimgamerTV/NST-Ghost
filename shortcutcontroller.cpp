#include "shortcutcontroller.h"

ShortcutController::ShortcutController(QMainWindow *parent)
    : QObject(parent), m_parent(parent)
{
}

void ShortcutController::createShortcuts()
{
    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_F), m_parent);
    connect(shortcut, &QShortcut::activated, this, &ShortcutController::focusSearch);
}

void ShortcutController::createSelectAllShortcut(QObject *parent)
{
    QShortcut *selectAllShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_A), parent);
    connect(selectAllShortcut, &QShortcut::activated, this, &ShortcutController::selectAllRequested);
}
