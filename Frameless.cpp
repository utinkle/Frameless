#include "Frameless.h"
#include "FramelessWorker.h"
#include "FramelessWorkerEvent.h"

#include <QWidget>
#include <QMouseEvent>
#include <QRect>
#include <QDebug>

Frameless::Frameless(QWidget *self)
    : mSelf(self)
    , mWorker(FramelessWorker::instance())
    , mCanWindowResize(false)
    , mDir(Direction::None)
    , mLeftButtonPress(false)
{
    mSelf->setWindowFlags(mSelf->windowFlags() | Qt::FramelessWindowHint);
    setCanWindowResize(true);
    mWorker->start();
}

void Frameless::setDirection(Direction dir)
{
    mDir = dir;
}

Frameless::Direction Frameless::direction() const
{
    return mDir;
}

void Frameless::setCanWindowResize(bool canResize)
{
    if (mCanWindowResize == canResize)
        return;

    mCanWindowResize = canResize;
    mSelf->setAttribute(Qt::WA_Hover, mCanWindowResize);
}

bool Frameless::canWindowResize() const
{
    return mCanWindowResize;
}

void Frameless::setDragPosition(const QPoint &dragPosition)
{
    mDragPosition = dragPosition;
}

QPoint Frameless::dragPosition() const
{
    return mDragPosition;
}

void Frameless::setLeftMouseButtonPressed(bool pressed)
{
    mLeftButtonPress = pressed;
}

bool Frameless::leftMouseButtonPressed() const
{
    return mLeftButtonPress;
}

void Frameless::event(QEvent *event)
{
    FramelessEvent *framelessEvent = nullptr;
    switch (event->type()) {
    case QEvent::HoverMove: {
        FramelessMouseHoverEvent *mouseHoverEvent = new FramelessMouseHoverEvent();
        mouseHoverEvent->globalCursorPositon = mSelf->mapToGlobal(static_cast<QHoverEvent *>(event)->pos());
        framelessEvent = mouseHoverEvent;

        event->accept();
    }
        break;
    case QEvent::MouseButtonPress: {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() != Qt::LeftButton)
            break;

        FramelessMousePressEvent *mousePressEvent = new FramelessMousePressEvent();
        mousePressEvent->globalCursorPositon = mouseEvent->globalPos();
        mousePressEvent->canWindowMove = canWindowMove();
        framelessEvent = mousePressEvent;

        mouseEvent->accept();
    }
        break;
    case QEvent::MouseButtonRelease: {
        FramelessMouseReleaseEvent *mouseReleaseEvent = new FramelessMouseReleaseEvent();
        framelessEvent = mouseReleaseEvent;
    }
        break;
    case QEvent::MouseMove: {
        FramelessMouseMoveEvent *mouseMoveEvent = new FramelessMouseMoveEvent();
        mouseMoveEvent->canWindowResize = mCanWindowResize;
        mouseMoveEvent->globalCursorPositon = static_cast<QMouseEvent *>(event)->globalPos();

        framelessEvent = mouseMoveEvent;
    }
        break;
    case QEvent::Leave: {
        FramelessLeaveEvent *leave = new FramelessLeaveEvent();
        framelessEvent = leave;
    }
        break;
    case QEvent::FocusIn: {
        FramelessFocusInEvent *focusInEvent = new FramelessFocusInEvent();
        focusInEvent->canWindowResize = mCanWindowResize;
        framelessEvent = focusInEvent;
    }
        break;
    default:
        break;
    }

    if (framelessEvent) {
        framelessEvent->target = mSelf;
        framelessEvent->frameless = this;

        mWorker->postEvent(framelessEvent);
    }
}
