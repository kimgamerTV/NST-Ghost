#include "loadprojectdialog.h"
#include "ui_loadprojectdialog.h"
#include <QFileDialog>
#include <QIcon>
#include <QStyle>

LoadProjectDialog::LoadProjectDialog(const QStringList &availableEngines, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoadProjectDialog)
{
    ui->setupUi(this);
    setWindowTitle("Load from Game Project");
    setMinimumSize(400, 150);

    // Populate engine combo box with icons
    for (const QString &engine : availableEngines) {
        QIcon icon;
        if (engine.compare("rpgm", Qt::CaseInsensitive) == 0) {
            icon = style()->standardIcon(QStyle::SP_DirIcon); // Generic folder icon for RPGM
        } else if (engine.compare("unity", Qt::CaseInsensitive) == 0) {
            icon = style()->standardIcon(QStyle::SP_DirIcon); // Generic folder icon for Unity
        } else {
            icon = style()->standardIcon(QStyle::SP_FileIcon); // Default file icon
        }
        ui->engineComboBox->addItem(icon, engine);
    }

    ui->projectPathLineEdit->setPlaceholderText("Select game project directory");

    connect(ui->browseButton, &QPushButton::clicked, this, &LoadProjectDialog::browseProjectPath);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &LoadProjectDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &LoadProjectDialog::reject);

    // Apply styling
    setStyleSheet("QDialog { background-color: #2d2d2d; color: #d4d4d4; border: 1px solid #007acc; border-radius: 5px; }"
                  "QLabel { color: #d4d4d4; }"
                  "QLineEdit { padding: 5px; border: 1px solid #3a3d41; border-radius: 3px; background-color: #3a3d41; color: #d4d4d4; }"
                  "QPushButton { padding: 5px 10px; border: 1px solid #007acc; border-radius: 3px; background-color: #007acc; color: #ffffff; }"
                  "QPushButton:hover { background-color: #005f99; }"
                  "QComboBox { padding: 5px; border: 1px solid #3a3d41; border-radius: 3px; background-color: #3a3d41; color: #d4d4d4; }"
                  "QComboBox::drop-down { border: none; }"
                  "QComboBox::down-arrow { image: url(:/icons/down_arrow.png); }" // Placeholder for an icon
                 );
}

LoadProjectDialog::~LoadProjectDialog()
{
    delete ui;
}

QString LoadProjectDialog::selectedEngine() const
{
    return ui->engineComboBox->currentText();
}

QString LoadProjectDialog::projectPath() const
{
    return ui->projectPathLineEdit->text();
}

void LoadProjectDialog::browseProjectPath()
{
    QString directory = QFileDialog::getExistingDirectory(this, "Select Game Project Directory",
                                                          QDir::homePath());
    if (!directory.isEmpty()) {
        ui->projectPathLineEdit->setText(directory);
    }
}