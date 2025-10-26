#include "customprogressdialog.h"

CustomProgressDialog::CustomProgressDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Loading Game Project...");
    setWindowModality(Qt::WindowModal);

    m_labelText = new QLabel("Initializing...", this);
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_labelText);
    layout->addWidget(m_progressBar);

    setLayout(layout);
}

CustomProgressDialog::~CustomProgressDialog()
{
}

void CustomProgressDialog::setValue(int value)
{
    m_progressBar->setValue(value);
}

void CustomProgressDialog::setLabelText(const QString &text)
{
    m_labelText->setText(text);
}