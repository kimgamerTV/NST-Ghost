#include "fontmanagerdialog.h"
#include "ui_fontmanagerdialog.h"
#include <QStringListModel>
#include <QJsonObject>

#include <QFontDatabase>

#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QFile>

FontManagerDialog::FontManagerDialog(const QJsonArray &fonts, const QString &targetLanguageName, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FontManagerDialog)
    , m_fonts(fonts)
    , m_targetLanguageName(targetLanguageName)
{
    ui->setupUi(this);

    QStringList fontNames;
    for (const QJsonValue &fontValue : m_fonts) {
        QJsonObject fontObject = fontValue.toObject();
        fontNames.append(fontObject["name"].toString());
    }

    QStringListModel *model = new QStringListModel(fontNames, this);
    ui->fontListView->setModel(model);

    connect(ui->addButton, &QPushButton::clicked, this, &FontManagerDialog::onAddButtonClicked);
    connect(ui->replaceButton, &QPushButton::clicked, this, &FontManagerDialog::onReplaceButtonClicked);

    connect(ui->fontListView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &FontManagerDialog::onFontSelectionChanged);
}

FontManagerDialog::~FontManagerDialog()
{
    delete ui;
}

void FontManagerDialog::onAddButtonClicked()
{
    QStringList extensions;
    if (!m_fonts.isEmpty()) {
        QJsonObject firstFont = m_fonts[0].toObject();
        QString firstPath = firstFont["path"].toString();
        extensions << QFileInfo(firstPath).suffix();
    }

    QString extension = extensions.isEmpty() ? "" : extensions[0];
    QString filter = tr("Font files (*.%1)").arg(extension);
    QString title = tr("Add Font (.%1)").arg(extension);

    QString filePath = QFileDialog::getOpenFileName(this, title, "", filter);

    if (!filePath.isEmpty()) {
        QFileInfo fileInfo(filePath);
        QString fontName = fileInfo.baseName();

        QJsonObject newFont;
        newFont["name"] = fontName;
        newFont["path"] = filePath;
        newFont["is_external"] = true; // Mark as external

        m_fonts.append(newFont);

        QStringListModel *model = static_cast<QStringListModel*>(ui->fontListView->model());
        QStringList currentNames = model->stringList();
        currentNames.append(fontName);
        model->setStringList(currentNames);
    }
}

void FontManagerDialog::onReplaceButtonClicked()
{
    QModelIndex currentIndex = ui->fontListView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, tr("Replace Font"), tr("Please select a font to replace."));
        return;
    }

    int selectedIndex = currentIndex.row();
    QJsonObject originalFont = m_fonts[selectedIndex].toObject();

    if (originalFont["is_external"].toBool()) {
        QMessageBox::warning(this, tr("Replace Font"), tr("Cannot replace an externally added font."));
        return;
    }

    QString originalPath = originalFont["path"].toString();
    QString extension = QFileInfo(originalPath).suffix();
    QString filter = tr("Font files (*.%1)").arg(extension);
    QString title = tr("Select New Font (.%1)").arg(extension);

    QString newPath = QFileDialog::getOpenFileName(this, title, "", filter);

    if (!newPath.isEmpty()) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("Replace Font"),
                                      tr("Are you sure you want to replace '%1' with '%2'?\nThe original file will be overwritten.")
                                      .arg(originalFont["name"].toString())
                                      .arg(QFileInfo(newPath).baseName()),
                                      QMessageBox::Yes|QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            if (QFile::remove(originalPath)) {
                if (!QFile::copy(newPath, originalPath)) {
                    QMessageBox::critical(this, tr("Error"), tr("Failed to copy the new font file."));
                    // Attempt to restore the original file if possible, though it's already deleted.
                }
            } else {
                QMessageBox::critical(this, tr("Error"), tr("Failed to remove the original font file."));
            }
        }
    }
}

void FontManagerDialog::onFontSelectionChanged(const QModelIndex &current, const QModelIndex &previous)
{
    if (!current.isValid()) {
        return;
    }

    int selectedIndex = current.row();
    if (selectedIndex >= 0 && selectedIndex < m_fonts.size()) {
        QJsonObject fontObject = m_fonts[selectedIndex].toObject();
        QString fontPath = fontObject["path"].toString();

        int fontId = QFontDatabase::addApplicationFont(fontPath);
        if (fontId != -1) {
            QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
            if (!fontFamilies.isEmpty()) {
                QFont font(fontFamilies.at(0), 24);
                ui->fontPreviewLabel->setFont(font);
            }
        }

        if (m_targetLanguageName.toLower() == "thai") {
            ui->fontPreviewLabel->setText("ทดสอบฟอนต์ภาษาไทย");
        } else {
            ui->fontPreviewLabel->setText("The quick brown fox jumps over the lazy dog");
        }
    }
}

