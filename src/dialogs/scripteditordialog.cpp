#include "scripteditordialog.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QTextDocument>

ScriptEditorDialog::ScriptEditorDialog(const QString &filePath, const QString &targetFunction, QWidget *parent)
    : QDialog(parent), m_filePath(filePath)
{
    setWindowTitle("Script Editor - " + filePath);
    resize(800, 600);

    auto *mainLayout = new QVBoxLayout(this);
    m_editor = new QPlainTextEdit(this);
    m_editor->setReadOnly(true); // Edit via external editor if needed, or make editable later. User asked for "edit", but for now viewing is safer to start, though user said "Edit rpg_windows.js". Let's make it editable but maybe just save on close or add a save button? User request: "แก้ไข rpg_windows.js ใน js ได้โดยทันที" -> "Edit ... immediately". So it should be editable.
    // Let's make it editable.
    m_editor->setReadOnly(false); 
    
    // Add Save Button? User request: "แก้ไข ... ได้โดยทันที" implies saving.
    // Minimally, I should probably add a save button.
    
    mainLayout->addWidget(m_editor);
    
    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *saveButton = new QPushButton("Save", this);
    QPushButton *closeButton = new QPushButton("Close", this);
    btnLayout->addStretch();
    btnLayout->addWidget(saveButton);
    btnLayout->addWidget(closeButton);
    mainLayout->addLayout(btnLayout);

    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(saveButton, &QPushButton::clicked, this, [this]() {
        QFile file(m_filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << m_editor->toPlainText();
            file.close();
            QMessageBox::information(this, "Success", "File saved successfully.");
        } else {
            QMessageBox::critical(this, "Error", "Could not save file.");
        }
    });

    loadFile(filePath);
    if (!targetFunction.isEmpty()) {
        scrollToFunction(targetFunction);
    }
}

void ScriptEditorDialog::loadFile(const QString &filePath)
{
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        m_editor->setPlainText(in.readAll());
        file.close();
    } else {
        m_editor->setPlainText("// Error loading file: " + filePath);
    }
}

void ScriptEditorDialog::scrollToFunction(const QString &functionName)
{
    QString content = m_editor->toPlainText();
    int index = content.indexOf(functionName);
    
    if (index != -1) {
        QTextCursor cursor = m_editor->textCursor();
        cursor.setPosition(index);
        m_editor->setTextCursor(cursor);
        m_editor->centerCursor();
    }
}
