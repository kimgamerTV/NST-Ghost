#include "processselectordialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDir>
#include <QTextStream>
#include <QDebug>

ProcessSelectorDialog::ProcessSelectorDialog(QWidget *parent)
    : QDialog(parent), m_selectedPid(-1)
{
    setWindowTitle("Select Target Process");
    resize(400, 500);
    setupUi();
    refreshProcesses();
}

qint64 ProcessSelectorDialog::selectedPid() const
{
    return m_selectedPid;
}

QString ProcessSelectorDialog::selectedName() const
{
    return m_selectedName;
}

void ProcessSelectorDialog::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Filter and Refresh
    QHBoxLayout *topLayout = new QHBoxLayout();
    m_filterEdit = new QLineEdit(this);
    m_filterEdit->setPlaceholderText("Filter processes...");
    connect(m_filterEdit, &QLineEdit::textChanged, this, &ProcessSelectorDialog::filterProcesses);
    
    m_refreshButton = new QPushButton("Refresh", this);
    connect(m_refreshButton, &QPushButton::clicked, this, &ProcessSelectorDialog::refreshProcesses);
    
    topLayout->addWidget(m_filterEdit);
    topLayout->addWidget(m_refreshButton);
    mainLayout->addLayout(topLayout);

    // List Widget
    m_processList = new QListWidget(this);
    connect(m_processList, &QListWidget::itemDoubleClicked, this, &ProcessSelectorDialog::onProcessSelected);
    connect(m_processList, &QListWidget::itemSelectionChanged, [this]() {
        m_selectButton->setEnabled(!m_processList->selectedItems().isEmpty());
    });
    mainLayout->addWidget(m_processList);

    // Buttons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    m_selectButton = new QPushButton("Select/Inject", this);
    m_selectButton->setEnabled(false);
    connect(m_selectButton, &QPushButton::clicked, this, &ProcessSelectorDialog::onProcessSelected);
    
    QPushButton *cancelButton = new QPushButton("Cancel", this);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    
    btnLayout->addStretch();
    btnLayout->addWidget(m_selectButton);
    btnLayout->addWidget(cancelButton);
    mainLayout->addLayout(btnLayout);
}

void ProcessSelectorDialog::refreshProcesses()
{
    m_allProcesses = getRunningProcesses();
    filterProcesses(m_filterEdit->text());
}

QList<ProcessInfo> ProcessSelectorDialog::getRunningProcesses()
{
    QList<ProcessInfo> list;
    QDir procDir("/proc");
    QStringList entries = procDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QString &entry : entries) {
        bool ok;
        qint64 pid = entry.toLongLong(&ok);
        if (!ok) continue;

        QFile commFile(QString("/proc/%1/comm").arg(entry)); // Short name
        QFile cmdlineFile(QString("/proc/%1/cmdline").arg(entry)); // Full command line

        QString name;
        if (commFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            name = QString::fromUtf8(commFile.readAll()).trimmed();
            commFile.close();
        }

        // Optional: Read full cmdline if needed for better identification (like "wine Game.exe")
        // For simplicity, we stick to name for now, but reading name is safer.
        
        if (!name.isEmpty()) {
            list.append({pid, name, ""});
        }
    }
    
    // Sort by name
    std::sort(list.begin(), list.end(), [](const ProcessInfo &a, const ProcessInfo &b) {
        return a.name.compare(b.name, Qt::CaseInsensitive) < 0;
    });

    return list;
}

void ProcessSelectorDialog::filterProcesses(const QString &text)
{
    m_processList->clear();
    for (const ProcessInfo &proc : m_allProcesses) {
        if (text.isEmpty() || proc.name.contains(text, Qt::CaseInsensitive) || QString::number(proc.pid).contains(text)) {
            QListWidgetItem *item = new QListWidgetItem(QString("%1 (%2)").arg(proc.name).arg(proc.pid));
            item->setData(Qt::UserRole, proc.pid);
            item->setData(Qt::UserRole + 1, proc.name);
            m_processList->addItem(item);
        }
    }
}

void ProcessSelectorDialog::onProcessSelected()
{
    auto items = m_processList->selectedItems();
    if (items.isEmpty()) return;
    
    m_selectedPid = items.first()->data(Qt::UserRole).toLongLong();
    m_selectedName = items.first()->data(Qt::UserRole + 1).toString();
    accept();
}
