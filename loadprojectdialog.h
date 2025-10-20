#ifndef LOADPROJECTDIALOG_H
#define LOADPROJECTDIALOG_H

#include <QDialog>
#include <QStringList>

namespace Ui {
class LoadProjectDialog;
}

class LoadProjectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoadProjectDialog(const QStringList &availableEngines, QWidget *parent = nullptr);
    ~LoadProjectDialog();

    QString selectedEngine() const;
    QString projectPath() const;

private slots:
    void browseProjectPath();

private:
    Ui::LoadProjectDialog *ui;
};

#endif // LOADPROJECTDIALOG_H