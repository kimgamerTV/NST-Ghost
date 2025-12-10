#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTreeWidget>
#include <QKeyEvent>
#include <QPair>
#include <QLabel>
#include <QPropertyAnimation>
#include <QTimer>

class SearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SearchDialog(QWidget *parent = nullptr);
    void displaySearchResults(const QList<QPair<QString, QPair<int, QString>>> &results, const QString &query);
    QLineEdit *lineEdit() const { return m_lineEdit; }

signals:
    void searchRequested(const QString &query);
    void resultSelected(const QString &fileName, int row);

private slots:
    void onResultSelected(QTreeWidgetItem *item, int intColumn);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    QLineEdit *m_lineEdit;
    QTreeWidget *m_resultsTreeWidget;
    QLabel *m_placeholderLabel;
    QPropertyAnimation *m_animation;
    QTimer *m_searchTimer;
};

#endif // SEARCHDIALOG_H