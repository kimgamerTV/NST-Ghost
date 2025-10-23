#ifndef FONTMANAGERDIALOG_H
#define FONTMANAGERDIALOG_H

#include <QDialog>
#include <QJsonArray>

QT_BEGIN_NAMESPACE
namespace Ui {
class FontManagerDialog;
}
QT_END_NAMESPACE

class FontManagerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FontManagerDialog(const QJsonArray &fonts, const QString &targetLanguageName, QWidget *parent = nullptr);
    ~FontManagerDialog();

private slots:
    void onFontSelectionChanged(const QModelIndex &current, const QModelIndex &previous);
    void onAddButtonClicked();
    void onReplaceButtonClicked();

private:
    Ui::FontManagerDialog *ui;
    QJsonArray m_fonts;
    QString m_targetLanguageName;
};

#endif // FONTMANAGERDIALOG_H
