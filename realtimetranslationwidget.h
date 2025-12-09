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
    void onLogMessage(const QString &message);
    void onNewTranslationRequest(const QString &sourceText, QTcpSocket *socket);

private:
    TranslationServer *m_server;
    QPushButton *m_toggleServerButton;
    QSpinBox *m_portSpinBox;
    QLabel *m_statusLabel;
    QTextEdit *m_logViewer;
    bool m_isServerRunning;
};

#endif // REALTIMETRANSLATIONWIDGET_H
