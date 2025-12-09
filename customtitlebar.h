#ifndef CUSTOMTITLEBAR_H
#define CUSTOMTITLEBAR_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>

class CustomTitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit CustomTitleBar(QWidget *parent = nullptr);

    void setTitle(const QString &title);
    void setIcon(const QIcon &icon);

signals:
    void minimizeClicked();
    void maximizeRestoreClicked();
    void closeClicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    QLabel *m_iconLabel;
    QLabel *m_titleLabel;
    QPushButton *m_minimizeButton;
    QPushButton *m_maximizeButton;
    QPushButton *m_closeButton;
    QHBoxLayout *m_layout;
    
    QPoint m_clickPos;
    bool m_isDrag;
};

#endif // CUSTOMTITLEBAR_H
