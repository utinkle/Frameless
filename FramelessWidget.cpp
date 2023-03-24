#include "FramelessWidget.h"
#include "Frameless.h"

#include <QScreen>
#include <QWindow>
#include <QDebug>
#include <QGraphicsEffect>
#include <QBoxLayout>

FramelessWidget::FramelessWidget(QWidget *parent)
    : QWidget(parent, Qt::Window)
    , mFrameless(new Frameless(this, this))
{
}

int FramelessWidget::framelessBorder() const
{
    return mFrameless->framelessBorder();
}

bool FramelessWidget::event(QEvent *e)
{
    if (e->type() == QEvent::MouseButtonPress
            || e->type() == QEvent::MouseMove) {
        mFrameless->setCanWindowMove(canWindowMove());
    }

    mFrameless->targetEvent(e);
    return QWidget::event(e);
}

void FramelessWidget::setCanWindowResize(bool canResize)
{
    mFrameless->setCanWindowResize(canResize);
}

bool FramelessWidget::canWindowResize() const
{
    return mFrameless->canWindowResize();
}

