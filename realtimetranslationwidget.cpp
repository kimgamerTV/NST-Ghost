#include "realtimetranslationwidget.h"

RealTimeTranslationWidget::RealTimeTranslationWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *label = new QLabel("Real-time Translation Mode (Coming Soon)", this);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("font-size: 18pt; color: #666;");
    layout->addWidget(label);
}
