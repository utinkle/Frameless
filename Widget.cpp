#include "Widget.h"

#include <QHBoxLayout>
#include <QLabel>

Widget::Widget(QWidget *parent)
    : FramelessWidget(parent)
{
    resize(800, 600);
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(0);

    QFrame *frame = new QFrame(this);
    frame->setLineWidth(5);
    frame->setFrameShape(QFrame::Box);
    mainLayout->addWidget(frame);
}

Widget::~Widget()
{
}
