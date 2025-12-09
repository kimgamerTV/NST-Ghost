#ifndef REALTIMETRANSLATIONWIDGET_H
#define REALTIMETRANSLATIONWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QPushButton>
#include <QTextEdit>
#include "translationserver.h"

class RealTimeTranslationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RealTimeTranslationWidget(QWidget *parent = nullptr);
    ~RealTimeTranslationWidget();

private slots:
    void onToggleServer();
    void onSelectProcess(); // New slot
    void onLogMessage(const QString &message);
    void onNewTranslationRequest(const QString &sourceText, QTcpSocket *socket);

private:
    TranslationServer *m_server;
    QPushButton *m_toggleServerButton;
    QPushButton *m_selectProcessButton; // New button
    QSpinBox *m_portSpinBox;
    QLabel *m_statusLabel;
    QTextEdit *m_logViewer;
    bool m_isServerRunning;
    qint64 m_targetPid; // Store selected PID
};

#endif // REALTIMETRANSLATIONWIDGET_H
