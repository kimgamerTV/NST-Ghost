#ifndef PROCESSSELECTORDIALOG_H
#define PROCESSSELECTORDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>

struct ProcessInfo {
    qint64 pid;
    QString name;
    QString cmdLine;
};

class ProcessSelectorDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ProcessSelectorDialog(QWidget *parent = nullptr);
    qint64 selectedPid() const;
    QString selectedName() const;

private slots:
    void refreshProcesses();
    void filterProcesses(const QString &text);
    void onProcessSelected();

private:
    void setupUi();
    QList<ProcessInfo> getRunningProcesses();

    QListWidget *m_processList;
    QLineEdit *m_filterEdit;
    QPushButton *m_refreshButton;
    QPushButton *m_selectButton;
    
    QList<ProcessInfo> m_allProcesses;
    qint64 m_selectedPid;
    QString m_selectedName;
};

#endif // PROCESSSELECTORDIALOG_H
