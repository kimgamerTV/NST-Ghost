#include "translationcontextmenu.h"
#include <QAction>
#include <QMessageBox>

TranslationContextMenu::TranslationContextMenu(TranslationServiceManager *manager, const QModelIndex &index, const QItemSelectionModel *selectionModel, QWidget *parent)
    : QMenu(parent)
    , m_serviceManager(manager)
    , m_index(index)
    , m_selectionModel(selectionModel)
{
    if (m_selectionModel && m_selectionModel->selectedRows().count() > 1) {
        QAction *translateAllAction = addAction("Translate Selected Rows");
        connect(translateAllAction, &QAction::triggered, this, &TranslationContextMenu::translateAllSelected);
    } else {
        QStringList availableServices = m_serviceManager->getAvailableServices();
        if (availableServices.isEmpty()) {
            addAction("No translation services available")->setEnabled(false);
        } else {
            for (const QString &serviceName : availableServices) {
                QAction *action = addAction(QString("Translate with %1").arg(serviceName));
                action->setData(serviceName); // Store service name in action data
                connect(action, &QAction::triggered, this, [this, serviceName]() {
                    // Get source text from the clicked index (column 0)
                    QString sourceText = m_index.sibling(m_index.row(), 0).data(Qt::DisplayRole).toString();
                    if (sourceText.isEmpty()) {
                        QMessageBox::information(this->parentWidget(), "Translate", "Selected cell has no source text to translate.");
                        return;
                    }
                    emit translateRequested(serviceName, sourceText);
                });
            }
        }
    }
}
