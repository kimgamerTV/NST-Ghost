#ifndef SCRIPTEDITORDIALOG_H
#define SCRIPTEDITORDIALOG_H

#include <QDialog>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

class ScriptEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScriptEditorDialog(const QString &filePath, const QString &targetFunction, QWidget *parent = nullptr);

private:
    void loadFile(const QString &filePath);
    void scrollToFunction(const QString &functionName);

    QPlainTextEdit *m_editor;
    QString m_filePath;
};

#endif // SCRIPTEDITORDIALOG_H
