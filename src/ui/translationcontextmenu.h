#ifndef TRANSLATIONCONTEXTMENU_H
#define TRANSLATIONCONTEXTMENU_H

#include <QMenu>
#include <QModelIndex>
#include <QItemSelectionModel>
#include "translationservicemanager.h"

class TranslationContextMenu : public QMenu
{
    Q_OBJECT
public:
    explicit TranslationContextMenu(TranslationServiceManager *manager, const QModelIndex &index, const QItemSelectionModel *selectionModel, QWidget *parent = nullptr);

signals:
    void translateRequested(const QString &serviceName, const QString &sourceText);
    void translateAllSelected();
    void undoTranslationRequested(); // New signal for undoing translation

private:
    TranslationServiceManager *m_serviceManager;
    QModelIndex m_index;
    const QItemSelectionModel *m_selectionModel;
};

#endif // TRANSLATIONCONTEXTMENU_H
