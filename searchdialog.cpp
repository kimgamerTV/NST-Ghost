#include "searchdialog.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QShowEvent>
#include <QMap>
#include <QPair>
#include <QJsonArray>
#include <QJsonObject>
#include <QFileInfo>

SearchDialog::SearchDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Search");
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint); // Reverted to FramelessWindowHint

    m_lineEdit = new QLineEdit(this);
    m_lineEdit->setPlaceholderText("Search...");

    m_resultsTreeWidget = new QTreeWidget(this);
    m_resultsTreeWidget->setHeaderHidden(true);

    m_placeholderLabel = new QLabel("Start typing to search...", this);
    m_placeholderLabel->setAlignment(Qt::AlignCenter);
    m_placeholderLabel->setStyleSheet("color: #808080; font-size: 12pt;");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5); // Add some margins
    layout->addWidget(m_lineEdit);
    layout->addWidget(m_placeholderLabel);
    layout->addWidget(m_resultsTreeWidget);
    setLayout(layout);

    // setFixedSize(400, 300); // Removed fixed size
    setMinimumSize(400, 100); // Set a reasonable minimum size

    // Apply styling to the dialog itself
    setStyleSheet("SearchDialog { background-color: #2d2d2d; border: 1px solid #007acc; border-radius: 5px; }"
                  "QLineEdit { padding: 5px; border: 1px solid #3a3d41; border-radius: 3px; background-color: #3a3d41; color: #d4d4d4; }"
                  "QTreeWidget { border: none; background-color: #2d2d2d; }"
                  "QTreeWidget::item { padding: 3px; }"
                  "QTreeWidget::item:selected { background-color: #007acc; color: #ffffff; }");

    m_animation = new QPropertyAnimation(m_placeholderLabel, "windowOpacity", this);
    m_animation->setDuration(200);

    connect(m_lineEdit, &QLineEdit::textChanged, this, &SearchDialog::onSearchQueryChanged);
    connect(m_lineEdit, &QLineEdit::returnPressed, this, &SearchDialog::onReturnPressed);
    connect(m_resultsTreeWidget, &QTreeWidget::itemClicked, this, &SearchDialog::onResultSelected);

    // Set tab order
    setTabOrder(m_lineEdit, m_resultsTreeWidget);
}

void SearchDialog::onSearchQueryChanged(const QString &query)
{
    emit searchRequested(query); // Emit for search across all files

    if (query.isEmpty()) {
        m_resultsTreeWidget->hide();
        m_placeholderLabel->show();
        m_animation->setStartValue(0.0);
        m_animation->setEndValue(1.0);
        m_animation->start();
    } else {
        m_placeholderLabel->hide();
    }
    adjustSize(); // Adjust size after query change
}

void SearchDialog::displaySearchResults(const QList<QPair<QString, QPair<int, QString>>> &results, const QString &query)
{
    m_resultsTreeWidget->clear();
    m_resultsTreeWidget->hide();

    if (query.isEmpty()) {
        m_placeholderLabel->show();
        adjustSize();
        return;
    }

    m_placeholderLabel->hide();

    QMap<QString, QTreeWidgetItem*> fileItems;

    for (const auto &result : results) {
        const QString &filePath = result.first;
        int row = result.second.first;
        const QString &matchingText = result.second.second;

        if (!fileItems.contains(filePath)) {
            QTreeWidgetItem *fileItem = new QTreeWidgetItem(m_resultsTreeWidget);
            fileItem->setText(0, QFileInfo(filePath).fileName());
            fileItem->setData(0, Qt::UserRole, filePath); // Store full path
            fileItems.insert(filePath, fileItem);
        }

        QTreeWidgetItem *fileItem = fileItems.value(filePath);
        QTreeWidgetItem *matchItem = new QTreeWidgetItem(fileItem);
        matchItem->setText(0, QString("Line %1: %2").arg(row + 1).arg(matchingText));
        matchItem->setData(0, Qt::UserRole, row); // Store row for later use
    }

    if (m_resultsTreeWidget->topLevelItemCount() > 0) {
        m_resultsTreeWidget->show();
        m_resultsTreeWidget->expandAll();
    } else {
        // No results found, display a message
        QTreeWidgetItem *noResultsItem = new QTreeWidgetItem(m_resultsTreeWidget);
        noResultsItem->setText(0, "No results found.");
        m_resultsTreeWidget->show();
    }

    adjustSize(); // Adjust size after populating results
}

void SearchDialog::onResultSelected(QTreeWidgetItem *item, int column)
{
    if (item->parent()) { // It's a match item, not a file item
        QString fileName = item->parent()->data(0, Qt::UserRole).toString(); // Full path
        int row = item->data(0, Qt::UserRole).toInt();
        emit resultSelected(fileName, row);
        close(); // Close the dialog after selecting a result
    } else { // It's a file item, just expand/collapse
        item->setExpanded(!item->isExpanded());
    }
}

void SearchDialog::onReturnPressed()
{
    // If line edit has focus, and there are results, move focus to tree widget
    if (m_lineEdit->hasFocus() && m_resultsTreeWidget->topLevelItemCount() > 0) {
        m_resultsTreeWidget->setFocus();
        if (m_resultsTreeWidget->currentItem() == nullptr) {
            m_resultsTreeWidget->setCurrentItem(m_resultsTreeWidget->topLevelItem(0));
        }
    } else { // Otherwise, if tree widget has focus, select the current item
        QTreeWidgetItem *selectedItem = m_resultsTreeWidget->currentItem();
        if (selectedItem) {
            onResultSelected(selectedItem, 0);
        }
    }
}

void SearchDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    m_lineEdit->clear(); // Clear previous search
    m_lineEdit->setFocus();
    m_resultsTreeWidget->hide();
    m_placeholderLabel->show();
    adjustSize(); // Adjust size when shown
}

bool SearchDialog::eventFilter(QObject *obj, QEvent *event)
{
    return QDialog::eventFilter(obj, event);
}

void SearchDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    } else {
        QDialog::keyPressEvent(event);
    }
}
