#include "customtitlebar.h"
#include <QStyle>
#include <QMouseEvent>
#include <QApplication>
#include <QButtonGroup>

CustomTitleBar::CustomTitleBar(QWidget *parent)
    : QWidget(parent), m_isDrag(false)
{
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(10, 0, 0, 0); // Left margin for icon/title
    m_layout->setSpacing(5);

    m_iconLabel = new QLabel(this);
    m_iconLabel->setFixedSize(20, 20);
    m_iconLabel->setScaledContents(true);

    m_titleLabel = new QLabel(this);
    m_titleLabel->setObjectName("titleBarTitle");
    
    // Navigation Buttons
    m_fileTransButton = new QPushButton(this);
    m_fileTransButton->setObjectName("navButton");
    m_fileTransButton->setText("File Translate");
    m_fileTransButton->setCheckable(true);
    m_fileTransButton->setChecked(true);
    m_fileTransButton->setCursor(Qt::PointingHandCursor);

    m_realTimeButton = new QPushButton(this);
    m_realTimeButton->setObjectName("navButton");
    m_realTimeButton->setText("Real-time");
    m_realTimeButton->setCheckable(true);
    m_realTimeButton->setCursor(Qt::PointingHandCursor);
    
    // Exclusive checking
    QButtonGroup *navGroup = new QButtonGroup(this);
    navGroup->addButton(m_fileTransButton);
    navGroup->addButton(m_realTimeButton);
    navGroup->setExclusive(true);
    
    // Window controls
    m_minimizeButton = new QPushButton(this);
    m_minimizeButton->setObjectName("titleBarMinimize");
    m_minimizeButton->setFixedSize(45, 30);
    m_minimizeButton->setText("─");
    m_minimizeButton->setToolTip("Minimize");

    m_maximizeButton = new QPushButton(this);
    m_maximizeButton->setObjectName("titleBarMaximize");
    m_maximizeButton->setFixedSize(45, 30);
    m_maximizeButton->setText("□");
    m_maximizeButton->setToolTip("Maximize");

    m_closeButton = new QPushButton(this);
    m_closeButton->setObjectName("titleBarClose");
    m_closeButton->setFixedSize(45, 30);
    m_closeButton->setText("✕");
    m_closeButton->setToolTip("Close");

    // Add widgets to layout
    m_layout->addWidget(m_iconLabel);
    m_layout->addWidget(m_titleLabel);
    m_layout->addSpacing(20);
    m_layout->addWidget(m_fileTransButton);
    m_layout->addWidget(m_realTimeButton);
    m_layout->addStretch();
    m_layout->addWidget(m_minimizeButton);
    m_layout->addWidget(m_maximizeButton);
    m_layout->addWidget(m_closeButton);

    // Style adjustments
    setFixedHeight(30);

    // Connect signals
    connect(m_minimizeButton, &QPushButton::clicked, this, &CustomTitleBar::minimizeClicked);
    connect(m_maximizeButton, &QPushButton::clicked, this, &CustomTitleBar::maximizeRestoreClicked);
    connect(m_closeButton, &QPushButton::clicked, this, &CustomTitleBar::closeClicked);
    
    connect(m_fileTransButton, &QPushButton::clicked, this, &CustomTitleBar::translateModeClicked);
    connect(m_realTimeButton, &QPushButton::clicked, this, &CustomTitleBar::realTimeModeClicked);
}

void CustomTitleBar::setTitle(const QString &title)
{
    m_titleLabel->setText(title);
}

void CustomTitleBar::setIcon(const QIcon &icon)
{
    m_iconLabel->setPixmap(icon.pixmap(20, 20));
}

void CustomTitleBar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_clickPos = event->globalPos();
        m_isDrag = true;
    }
    QWidget::mousePressEvent(event);
}

void CustomTitleBar::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isDrag && (event->globalPos() - m_clickPos).manhattanLength() > QApplication::startDragDistance()) {
       // Drag logic is handled in MainWindow because we need to move the whole window
       // But we still emit a signal or handle it if we want to move here.
       // Actually, standard way is to handle move in MainWindow by checking where the press happened.
       // However, often it's easier to just handle it here if we pass the parent window.
       if (window()->isMaximized()) {
           // Optional: Implement "snap out of maximize" behavior like Windows
           return; 
       }
       window()->move(window()->pos() + event->globalPos() - m_clickPos);
       m_clickPos = event->globalPos();
    }
    QWidget::mouseMoveEvent(event);
}

void CustomTitleBar::mouseReleaseEvent(QMouseEvent *event)
{
    m_isDrag = false;
    QWidget::mouseReleaseEvent(event);
}

void CustomTitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit maximizeRestoreClicked();
    }
    QWidget::mouseDoubleClickEvent(event);
}
