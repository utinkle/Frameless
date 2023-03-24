#include "FramelessWorker.h"
#include "Frameless.h"
#include "FramelessWorkerEvent.h"

#include <QWidget>
#include <QApplication>
#include <QDebug>
#include <QLayout>

FramelessWorker *FramelessWorker::mInstance = nullptr;

FramelessWorker::FramelessWorker(QObject *parent)
    : QThread{parent}
    , mExit(false)
{
    connect(qApp, &QApplication::aboutToQuit, this, &FramelessWorker::exit);
}

FramelessWorker *FramelessWorker::instance()
{
    if (mInstance)
        return mInstance;

    return new FramelessWorker;
}

void FramelessWorker::exit()
{
    mMutex.lock();
    mExit = true;
    mCondition.wakeAll();
    mMutex.unlock();
    wait();
}

void FramelessWorker::postEvent(FramelessEvent *event)
{
    mMutex.lock();
    mEventQueue.append(event);
    mCondition.wakeAll();
    mMutex.unlock();
}

FramelessEvent *FramelessWorker::takeEvent()
{
    QMutexLocker locker(&mMutex);
    while (mEventQueue.isEmpty() && !mExit)
        mCondition.wait(&mMutex);

    FramelessEvent *event = nullptr;
    if (mEventQueue.length() > 0)
        event = mEventQueue.takeFirst();

    return event;
}

void FramelessWorker::run()
{
    while (true) {
        FramelessEvent *event = takeEvent();
        if (mExit) {
            delete event;
            break;
        }

        if (event->type() == FramelessEvent::UnkonwEvent) {
            delete event;
            continue;
        }

        switch (event->type()) {
        case FramelessEvent::FocusIn:
            focusIn(static_cast<FramelessFocusInEvent *>(event));
            break;

        case FramelessEvent::MouseHover:
            mouseHover(static_cast<FramelessMouseHoverEvent *>(event));
            break;

        case FramelessEvent::MousePress:
            mousePress(static_cast<FramelessMousePressEvent *>(event));
            break;

        case FramelessEvent::MouseMove:
            mouseMove(static_cast<FramelessMouseMoveEvent *>(event));
            break;

        case FramelessEvent::MouseRelease:
            mouseRelease(static_cast<FramelessMouseReleaseEvent *>(event));
            break;

        case FramelessEvent::Leave:
            leave(static_cast<FramelessLeaveEvent *>(event));
            break;

        default:
            break;
        }

        delete event;
    }
}
FramelessWorker::DirAndCursorShape FramelessWorker::calcDirAndCursorShape(const QRect &rOrigin, const QPoint &cursorGlobalPoint, int framelessBorder)
{
    int x = cursorGlobalPoint.x();
    int y = cursorGlobalPoint.y();

    DirAndCursorShape dirAndShape;
    if (rOrigin.x() + framelessBorder >= x
            && rOrigin.x() <= x
            && rOrigin.y() + framelessBorder >= y
            && rOrigin.y() <= y) {
        dirAndShape.dir = static_cast<int>(Frameless::Direction::TopLeft);
        dirAndShape.cursorShape = Qt::SizeFDiagCursor;
    } else if (x >= rOrigin.right() - framelessBorder
              && x <= rOrigin.right()
              && y >= rOrigin.bottom() - framelessBorder
              && y <= rOrigin.bottom()) {
        dirAndShape.dir = static_cast<int>(Frameless::Direction::BottomRight);
        dirAndShape.cursorShape = Qt::SizeFDiagCursor;
    } else if (x <= rOrigin.x() + framelessBorder
              && x >= rOrigin.x()
              && y >= rOrigin.bottom() - framelessBorder
              && y <= rOrigin.bottom()) {
        dirAndShape.dir = static_cast<int>(Frameless::Direction::BottomLeft);
        dirAndShape.cursorShape = Qt::SizeBDiagCursor;
    } else if (x <= rOrigin.right()
              && x >= rOrigin.right() - framelessBorder
              && y >= rOrigin.y()
              && y <= rOrigin.y() + framelessBorder) {
        dirAndShape.dir = static_cast<int>(Frameless::Direction::TopRight);
        dirAndShape.cursorShape = Qt::SizeBDiagCursor;
    } else if (x <= rOrigin.x() + framelessBorder && x >= rOrigin.x()) {
        dirAndShape.dir = static_cast<int>(Frameless::Direction::Left);
        dirAndShape.cursorShape = Qt::SizeHorCursor;
    } else if (x <= rOrigin.right() && x >= rOrigin.right() - framelessBorder) {
        dirAndShape.dir = static_cast<int>(Frameless::Direction::Right);
        dirAndShape.cursorShape = Qt::SizeHorCursor;
    } else if (y >= rOrigin.y() && y <= rOrigin.y() + framelessBorder) {
        dirAndShape.dir = static_cast<int>(Frameless::Direction::Up);
        dirAndShape.cursorShape = Qt::SizeVerCursor;
    } else if (y <= rOrigin.bottom() && y >= rOrigin.bottom() - framelessBorder) {
        dirAndShape.dir = static_cast<int>(Frameless::Direction::Down);
        dirAndShape.cursorShape = Qt::SizeVerCursor;
    } else {
        dirAndShape.dir = static_cast<int>(Frameless::Direction::None);
        dirAndShape.cursorShape = Qt::ArrowCursor;
    }

    return dirAndShape;
}

QRect FramelessWorker::calcPositionRect(int dir, QWidget *target, const QRect &rOrigin, const QPoint &gloPoint)
{
    QRect rMove(rOrigin);

    switch (static_cast<Frameless::Direction>(dir)) {
    case Frameless::Direction::Left: {
        if (rOrigin.right() - gloPoint.x() <= target->minimumWidth()) {
            rMove.setX(rOrigin.left());
        } else {
            rMove.setX(gloPoint.x());
        }
    }
        break;

    case Frameless::Direction::Right: {
        if (gloPoint.x() - rOrigin.left() <= target->minimumWidth()) {
            rMove.setX(rOrigin.left());
        } else {
            rMove.setX(rOrigin.left());
            rMove.setWidth(gloPoint.x() - rOrigin.left());
        }
    }
        break;

    case Frameless::Direction::Up: {
        if (rOrigin.bottom() - gloPoint.y() <= target->minimumHeight()) {
            rMove.setY(rOrigin.top());
        } else {
            rMove.setY(gloPoint.y());

        }
    }
        break;

    case Frameless::Direction::Down: {
        if (gloPoint.y() - rOrigin.top() <= target->minimumHeight()) {
            rMove.setY(rOrigin.top());
        } else {
            rMove.setY(rOrigin.top());
            rMove.setHeight(gloPoint.y() - rOrigin.top());
        }
    }
        break;

    case Frameless::Direction::TopLeft: {
        if (rOrigin.right() - gloPoint.x() <= target->minimumWidth()) {
            rMove.setX(rOrigin.left());
        } else {
            rMove.setX(gloPoint.x());
        }

        if (rOrigin.bottom() - gloPoint.y() <= target->minimumHeight()) {
            rMove.setY(rOrigin.top());
        } else {
            rMove.setY(gloPoint.y());
        }
    }
        break;

    case Frameless::Direction::TopRight: {
        if (gloPoint.x() - rOrigin.left() <= target->minimumWidth()) {
            rMove.setX(rOrigin.left());
        } else {
            rMove.setX(rOrigin.left());
            rMove.setWidth(gloPoint.x() - rOrigin.left());
        }

        if (rOrigin.bottom() - gloPoint.y() <= target->minimumHeight()) {
            rMove.setY(rOrigin.bottom());
        } else {
            rMove.setY(gloPoint.y());
        }
    }
        break;

    case Frameless::Direction::BottomLeft: {
        if (rOrigin.right() - gloPoint.x() <= target->minimumWidth()) {
            rMove.setX(rOrigin.left());
        } else {
            rMove.setX(gloPoint.x());
        }

        if (gloPoint.y() - rOrigin.top() <= target->minimumHeight()) {
            rMove.setY(rOrigin.top());
        } else {
            rMove.setY(rOrigin.top());
            rMove.setHeight(gloPoint.y() - rOrigin.top());
        }
    }
        break;

    case Frameless::Direction::BottomRight: {
        if (gloPoint.x() - rOrigin.left() <= target->minimumWidth()) {
            rMove.setX(rOrigin.left());
        } else {
            rMove.setX(rOrigin.left());
            rMove.setWidth(gloPoint.x() - rOrigin.left());
        }

        if (gloPoint.y() - rOrigin.top() <= target->minimumHeight()) {
            rMove.setY(rOrigin.top());
        } else {
            rMove.setY(rOrigin.top());
            rMove.setHeight(gloPoint.y() - rOrigin.top());
        }
    }
        break;

    default:
        break;
    }

    return rMove;
}

QRect FramelessWorker::calcOriginRect(QWidget *taget)
{
    const QRect &rect = taget->frameGeometry();
    QPoint tl = rect.topLeft();
    QPoint rb = rect.bottomRight();

    QWidget *window = taget->window();
    if (window && window != taget) {
        tl = taget->mapTo(window, taget->mapFromGlobal(tl));
        rb = taget->mapTo(window, taget->mapFromGlobal(rb));
    }

    return QRect(tl, rb);
}

QPoint FramelessWorker::calcFakeGlobalPos(QWidget *taget, const QPoint &gloPoint)
{
    QPoint gp = gloPoint;
    QWidget *window = taget->window();
    if (window && window != taget) {
        gp = taget->mapTo(window, taget->mapFromGlobal(gloPoint));
    }

    return gp;
}

void FramelessWorker::focusIn(FramelessFocusInEvent *event)
{
    if (!event->canWindowResize)
        return;

    DirAndCursorShape dirAndShape = calcDirAndCursorShape(calcOriginRect(event->target),
                                                          calcFakeGlobalPos(event->target, event->target->cursor().pos()),
                                                          event->frameless->framelessBorder());
    QMetaObject::invokeMethod(event->frameless, "setCursorByFrameless", Qt::QueuedConnection,
                              Q_ARG(int, int(dirAndShape.cursorShape)));
    event->frameless->setDirection(static_cast<Frameless::Direction>(dirAndShape.dir));
}

void FramelessWorker::mouseHover(FramelessMouseHoverEvent *event)
{

    if (event->frameless->leftMouseButtonPressed()
            || !event->canWindowResize)
        return;

    QWidget *window = event->target->window();
    if (!window || (window == event->target && window->isMaximized()))
        return;

    QRect rect = calcOriginRect(event->target);
    if (QLayout *layout = event->target->layout()) {
        int margin = layout->margin();
        rect.adjust(margin, margin, -margin, -margin);
    }

    DirAndCursorShape dirAndShape = calcDirAndCursorShape(rect, calcFakeGlobalPos(event->target, event->globalCursorPositon),
                                                          event->frameless->framelessBorder());
    event->frameless->setDirection(static_cast<Frameless::Direction>(dirAndShape.dir));
    QMetaObject::invokeMethod(event->frameless, "setCursorByFrameless", Qt::QueuedConnection,
                              Q_ARG(int, int(dirAndShape.cursorShape)));
}


void FramelessWorker::mousePress(FramelessMousePressEvent *event)
{
    event->frameless->setLeftMouseButtonPressed(true);

    if (event->frameless->direction() == Frameless::Direction::None) {
        if (event->canWindowMove) {
            event->frameless->setDragPosition(event->globalCursorPositon - event->target->frameGeometry().topLeft());
            QMetaObject::invokeMethod(event->frameless, "setCursorByFrameless", Qt::QueuedConnection,
                                      Q_ARG(int, int(Qt::SizeAllCursor)));
        }
    } else {
        event->target->releaseMouse();
    }
}

void FramelessWorker::mouseMove(FramelessMouseMoveEvent *event)
{
    QPoint gloPoint = event->globalCursorPositon;

    if (event->frameless->leftMouseButtonPressed()
            && (event->frameless->direction() == Frameless::Direction::None)
            && event->canWindowMove) {
        if (event->target->isMaximized()) {
            // event->target->showNormal();

            // double xRatio = event->globalX() * 1.0 / event->target->width();
            // double yRatio = event->globalY() * 1.0 / event->target->height();
            // int widgetX = event->target->width() * xRatio;
            // int widgetY = event->target->height() * yRatio;

            // event->target->move(event->globalX() - widgetX, event->globalY() - widgetY);
            // event->target->layout()->setMargin(10);

            // dragPosition = event->globalPos() - event->target->frameGeometry().topLeft();
            return;
        }

        QMetaObject::invokeMethod(event->frameless, "moveByFrameless", Qt::QueuedConnection,
                                  Q_ARG(QPoint, event->globalCursorPositon - event->frameless->dragPosition()));
        return;
    }

    if (event->frameless->leftMouseButtonPressed() && event->canWindowResize) {
        gloPoint = calcFakeGlobalPos(event->target, gloPoint);

        const QRect &rect = calcPositionRect(static_cast<int>(event->frameless->direction()),
                                             event->target, calcOriginRect(event->target), gloPoint);
        QMetaObject::invokeMethod(event->frameless, "setGeometryByFrameless", Qt::QueuedConnection,
                                  Q_ARG(QRect, rect));
    }
}

void FramelessWorker::mouseRelease(FramelessMouseReleaseEvent *event)
{
    QMetaObject::invokeMethod(event->frameless, "unsetCursorByFrameless", Qt::QueuedConnection);
    event->frameless->setLeftMouseButtonPressed(false);
    event->frameless->setDirection(Frameless::Direction::None);
    event->frameless->setDragPosition({0, 0});
}

void FramelessWorker::leave(FramelessLeaveEvent *event)
{
    if (event->frameless->leftMouseButtonPressed())
        return;

    event->frameless->setDirection(Frameless::Direction::None);
    QMetaObject::invokeMethod(event->frameless, "unsetCursorByFrameless", Qt::QueuedConnection);
}
