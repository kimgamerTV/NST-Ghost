#include "fontmanagerdialog.h"
#include "ui_fontmanagerdialog.h"
#include <QStringListModel>
#include <QJsonObject>
#include <QFontDatabase>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QFile>
#include <QDir>

FontManagerDialog::FontManagerDialog(const QJsonArray &fonts, const QString &targetLanguageName, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FontManagerDialog)
    , m_fonts(fonts)
    , m_targetLanguageName(targetLanguageName)
    , m_currentFontId(-1)  // เพิ่ม member variable
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
    // ปล่อย font ที่โหลดไว้
    if (m_currentFontId != -1) {
        QFontDatabase::removeApplicationFont(m_currentFontId);
    }
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

    QString extension = extensions.isEmpty() ? "ttf" : extensions[0];
    QString filter = tr("Font files (*.%1);;All files (*)").arg(extension);
    QString title = tr("Add Font (.%1)").arg(extension);

    QString filePath = QFileDialog::getOpenFileName(this, title, QString(), filter);

    if (!filePath.isEmpty()) {
        // แปลง path ให้เหมาะกับ OS
        filePath = QDir::toNativeSeparators(filePath);

        // ตรวจสอบว่าไฟล์มีอยู่จริง
        if (!QFile::exists(filePath)) {
            QMessageBox::warning(this, tr("Error"), tr("Font file does not exist."));
            return;
        }

        QFileInfo fileInfo(filePath);
        QString fontName = fileInfo.baseName();

        QJsonObject newFont;
        newFont["name"] = fontName;
        newFont["path"] = filePath;
        newFont["is_external"] = true;

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

    QString originalPath = QDir::toNativeSeparators(originalFont["path"].toString());
    QString extension = QFileInfo(originalPath).suffix();
    QString filter = tr("Font files (*.%1);;All files (*)").arg(extension);
    QString title = tr("Select New Font (.%1)").arg(extension);

    QString newPath = QFileDialog::getOpenFileName(this, title, QString(), filter);

    if (!newPath.isEmpty()) {
        newPath = QDir::toNativeSeparators(newPath);

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("Replace Font"),
                                      tr("Are you sure you want to replace '%1' with '%2'?\nThe original file will be overwritten.")
                                          .arg(originalFont["name"].toString())
                                          .arg(QFileInfo(newPath).baseName()),
                                      QMessageBox::Yes|QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            // ปล่อย font ที่กำลังใช้อยู่ก่อน
            if (m_currentFontId != -1) {
                QFontDatabase::removeApplicationFont(m_currentFontId);
                m_currentFontId = -1;
            }

            // สำรองชื่อไฟล์เดิม
            QString backupPath = originalPath + ".backup";

            // Rename แทนการลบทันที (ปลอดภัยกว่า)
            if (QFile::rename(originalPath, backupPath)) {
                if (QFile::copy(newPath, originalPath)) {
                    // สำเร็จ ลบ backup
                    QFile::remove(backupPath);
                    QMessageBox::information(this, tr("Success"), tr("Font replaced successfully."));
                } else {
                    // ล้มเหลว คืนไฟล์เดิม
                    QFile::rename(backupPath, originalPath);
                    QMessageBox::critical(this, tr("Error"), tr("Failed to copy the new font file."));
                }
            } else {
                QMessageBox::critical(this, tr("Error"),
                                      tr("Failed to replace font. The file may be in use.\nPlease restart the application and try again."));
            }
        }
    }
}

void FontManagerDialog::onFontSelectionChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);

    if (!current.isValid()) {
        return;
    }

    int selectedIndex = current.row();
    if (selectedIndex < 0 || selectedIndex >= m_fonts.size()) {
        return;
    }

    // ปล่อย font เดิมก่อน
    if (m_currentFontId != -1) {
        QFontDatabase::removeApplicationFont(m_currentFontId);
        m_currentFontId = -1;
    }

    QJsonObject fontObject = m_fonts[selectedIndex].toObject();
    QString fontPath = QDir::toNativeSeparators(fontObject["path"].toString());

    // ตรวจสอบว่าไฟล์มีอยู่จริง
    if (!QFile::exists(fontPath)) {
        ui->fontPreviewLabel->setText(tr("Font file not found: %1").arg(fontPath));
        ui->fontPreviewLabel->setFont(QFont("Arial", 10));
        return;
    }

    m_currentFontId = QFontDatabase::addApplicationFont(fontPath);
    if (m_currentFontId != -1) {
        QStringList fontFamilies = QFontDatabase::applicationFontFamilies(m_currentFontId);
        if (!fontFamilies.isEmpty()) {
            QString fontName = fontFamilies.at(0);
            QFont font(fontName, 24);
            ui->fontPreviewLabel->setFont(font);
        } else {
            // Fallback ถ้าโหลดฟอนต์ไม่สำเร็จ
            ui->fontPreviewLabel->setFont(QFont("Arial", 24));
        }
    } else {
        // แสดง error ถ้าโหลดฟอนต์ไม่สำเร็จ
        ui->fontPreviewLabel->setText(tr("Failed to load font: %1").arg(fontObject["name"].toString()));
        ui->fontPreviewLabel->setFont(QFont("Arial", 10));
        return;
    }

    // ตั้งข้อความ preview
    if (m_targetLanguageName.toLower() == "thai") {
        ui->fontPreviewLabel->setText(QStringLiteral("ทดสอบฟอนต์ภาษาไทย 0123456789"));
    } else {
        ui->fontPreviewLabel->setText(QStringLiteral("The quick brown fox jumps over the lazy dog 0123456789"));
    }
}
