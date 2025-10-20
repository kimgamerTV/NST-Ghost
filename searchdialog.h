#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTreeWidget>
#include <QKeyEvent>
#include <QPair>
#include <QLabel>
#include <QPropertyAnimation>

class SearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SearchDialog(QWidget *parent = nullptr);
    void displaySearchResults(const QList<QPair<QString, QPair<int, QString>>> &results, const QString &query);

signals:
    void searchRequested(const QString &query);
    void resultSelected(const QString &fileName, int row);

private slots:
    void onSearchQueryChanged(const QString &query);
    void onResultSelected(QTreeWidgetItem *item, int intColumn);
    void onReturnPressed();

protected:
    void showEvent(QShowEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    QLineEdit *m_lineEdit;
    QTreeWidget *m_resultsTreeWidget;
    QLabel *m_placeholderLabel;
    QPropertyAnimation *m_animation;
};

#endif // SEARCHDIALOG_H