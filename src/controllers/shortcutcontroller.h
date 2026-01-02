#ifndef SHORTCUTCONTROLLER_H
#define SHORTCUTCONTROLLER_H

#include <QObject>
#include <QShortcut>
#include <QMainWindow>

class ShortcutController : public QObject
{
    Q_OBJECT
public:
    explicit ShortcutController(QMainWindow *parent = nullptr);
    void createShortcuts();
    void createSelectAllShortcut(QObject *parent);

signals:
    void focusSearch();
    void selectAllRequested();

private:
    QMainWindow *m_parent;
};

#endif // SHORTCUTCONTROLLER_H
