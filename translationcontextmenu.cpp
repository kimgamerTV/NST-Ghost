#include "translationcontextmenu.h"
#include <QAction>
#include <QMessageBox>

TranslationContextMenu::TranslationContextMenu(TranslationServiceManager *manager, const QModelIndex &index, const QItemSelectionModel *selectionModel, QWidget *parent)
    : QMenu(parent)
    , m_serviceManager(manager)
    , m_index(index)
    , m_selectionModel(selectionModel)
{
    // Always add "Undo Selected Translations" if any rows are selected
    if (m_selectionModel && m_selectionModel->selectedRows().count() >= 1) {
        QAction *undoAction = addAction("Undo Selected Translations");
        connect(undoAction, &QAction::triggered, this, &TranslationContextMenu::undoTranslationRequested);
    }

    // Add translation options
    if (m_selectionModel && m_selectionModel->selectedRows().count() > 1) { // For multiple selections, offer "Translate Selected Rows"
        QAction *translateAllAction = addAction("Translate Selected Rows");
        connect(translateAllAction, &QAction::triggered, this, &TranslationContextMenu::translateAllSelected);
    } else {
        // For single selection, offer "Translate with Service"
        QStringList availableServices = m_serviceManager->getAvailableServices(); // Get services here
        if (!availableServices.isEmpty()) { // Only add if services are available
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
        } else {
            addAction("No translation services available")->setEnabled(false);
        }
    }
}
