#include "Frameless.h"

#include <QWidget>
#include <QMouseEvent>
#include <QRect>
#include <QDebug>

#define PADDING 4

Frameless::Frameless(QWidget *self)
    : mSelf(self)
    , mDir(Direction::None)
    , mLeftButtonPress(false)
    , mCanWindowResize(false)
{
    mSelf->setWindowFlags(mSelf->windowFlags() | Qt::FramelessWindowHint);
    setCanWindowResize(true);
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

void Frameless::calcDirAndCursorShape(const QRect &rOrigin, const QPoint &cursorGlobalPoint)
{
    int x = cursorGlobalPoint.x();
    int y = cursorGlobalPoint.y();
    Qt::CursorShape cursorShape = Qt::ArrowCursor;

    if (rOrigin.x() + PADDING >= x
            && rOrigin.x() <= x
            && rOrigin.y() + PADDING >= y
            && rOrigin.y() <= y) {
        this->mDir = Direction::TopLeft;
        cursorShape = Qt::SizeFDiagCursor;
    } else if (x >= rOrigin.right() - PADDING
              && x <= rOrigin.right()
              && y >= rOrigin.bottom() - PADDING
              && y <= rOrigin.bottom()) {
        this->mDir = Direction::BottomRight;
        cursorShape = Qt::SizeFDiagCursor;
    } else if (x <= rOrigin.x() + PADDING
              && x >= rOrigin.x()
              && y >= rOrigin.bottom() - PADDING
              && y <= rOrigin.bottom()) {
        this->mDir = Direction::BottomLeft;
        cursorShape = Qt::SizeBDiagCursor;
    } else if (x <= rOrigin.right()
              && x >= rOrigin.right() - PADDING
              && y >= rOrigin.y()
              && y <= rOrigin.y() + PADDING) {
        this->mDir = Direction::TopRight;
        cursorShape = Qt::SizeBDiagCursor;
    } else if (x <= rOrigin.x() + PADDING && x >= rOrigin.x()) {
        this->mDir = Direction::Left;
        cursorShape = Qt::SizeHorCursor;
    } else if (x <= rOrigin.right() && x >= rOrigin.right() - PADDING) {
        this->mDir = Direction::Right;
        cursorShape = Qt::SizeHorCursor;
    } else if (y >= rOrigin.y() && y <= rOrigin.y() + PADDING) {
        this->mDir = Direction::Up;
        cursorShape = Qt::SizeVerCursor;
    } else if (y <= rOrigin.bottom() && y >= rOrigin.bottom() - PADDING) {
        this->mDir = Direction::Down;
        cursorShape = Qt::SizeVerCursor;
    } else {
        this->mDir = Direction::None;
        cursorShape = Qt::ArrowCursor;
    }

    mSelf->setCursor(QCursor(cursorShape));
}

QRect Frameless::calcPositionRect(const QRect &rOrigin, const QPoint &gloPoint)
{
    QRect rMove(rOrigin);

    switch (this->mDir) {
    case Direction::Left: {
        if (rOrigin.right() - gloPoint.x() <= mSelf->minimumWidth()) {
            rMove.setX(rOrigin.left());
        } else {
            rMove.setX(gloPoint.x());
        }
    }
        break;
    case Direction::Right: {
        if (gloPoint.x() - rOrigin.left() <= mSelf->minimumWidth()) {
            rMove.setX(rOrigin.left());
        } else {
            rMove.setX(rOrigin.left());
            rMove.setWidth(gloPoint.x() - rOrigin.left());
        }
    }
        break;
    case Direction::Up: {
        if (rOrigin.bottom() - gloPoint.y() <= mSelf->minimumHeight()) {
            rMove.setY(rOrigin.top());
        } else {
            rMove.setY(gloPoint.y());

        }
    }
        break;
    case Direction::Down: {
        if (gloPoint.y() - rOrigin.top() <= mSelf->minimumHeight()) {
            rMove.setY(rOrigin.top());
        } else {
            rMove.setY(rOrigin.top());
            rMove.setHeight(gloPoint.y() - rOrigin.top());
        }
    }
        break;
    case Direction::TopLeft: {
        if (rOrigin.right() - gloPoint.x() <= mSelf->minimumWidth()) {
            rMove.setX(rOrigin.left());
        } else {
            rMove.setX(gloPoint.x());
        }

        if (rOrigin.bottom() - gloPoint.y() <= mSelf->minimumHeight()) {
            rMove.setY(rOrigin.top());
        } else {
            rMove.setY(gloPoint.y());
        }
    }
        break;
    case Direction::TopRight: {
        if (gloPoint.x() - rOrigin.left() <= mSelf->minimumWidth()) {
            rMove.setX(rOrigin.left());
        } else {
            rMove.setX(rOrigin.left());
            rMove.setWidth(gloPoint.x() - rOrigin.left());
        }

        if (rOrigin.bottom() - gloPoint.y() <= mSelf->minimumHeight()) {
            rMove.setY(rOrigin.bottom());
        } else {
            rMove.setY(gloPoint.y());
        }
    }
        break;
    case Direction::BottomLeft: {
        if (rOrigin.right() - gloPoint.x() <= mSelf->minimumWidth()) {
            rMove.setX(rOrigin.left());
        } else {
            rMove.setX(gloPoint.x());
        }

        if (gloPoint.y() - rOrigin.top() <= mSelf->minimumHeight()) {
            rMove.setY(rOrigin.top());
        } else {
            rMove.setY(rOrigin.top());
            rMove.setHeight(gloPoint.y() - rOrigin.top());
        }
    }
        break;
    case Direction::BottomRight: {
        if (gloPoint.x() - rOrigin.left() <= mSelf->minimumWidth()) {
            rMove.setX(rOrigin.left());
        } else {
            rMove.setX(rOrigin.left());
            rMove.setWidth(gloPoint.x() - rOrigin.left());
        }

        if (gloPoint.y() - rOrigin.top() <= mSelf->minimumHeight()) {
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

QRect Frameless::calcOriginRect()
{
    const QRect &rect = mSelf->frameGeometry();
    QPoint tl = rect.topLeft();
    QPoint rb = rect.bottomRight();

    QWidget *window = mSelf->window();
    if (window && window != mSelf) {
        tl = mSelf->mapTo(window, mSelf->mapFromGlobal(tl));
        rb = mSelf->mapTo(window, mSelf->mapFromGlobal(rb));
    }

    return QRect(tl, rb);
}

QPoint Frameless::calcFakeGlobalPos(const QPoint &gloPoint)
{
    QPoint gp = gloPoint;
    QWidget *window = mSelf->window();
    if (window && window != mSelf) {
        gp = mSelf->mapTo(window, mSelf->mapFromGlobal(gloPoint));
    }

    return gp;
}

void Frameless::focusIn()
{
    if (!mCanWindowResize)
        return;

    calcDirAndCursorShape(calcOriginRect(), calcFakeGlobalPos(mSelf->cursor().pos()));
}

void Frameless::mouseHover(const QPoint &globalPos)
{
    if (this->mLeftButtonPress)
        return;

    calcDirAndCursorShape(calcOriginRect(), calcFakeGlobalPos(globalPos));
}


void Frameless::mousePress(const QPoint &globalPos)
{
    this->mPoint = globalPos;
    this->mLeftButtonPress = true;

    if (this->mDir == Direction::None) {
        if (canWindowMove()) {
            mDragPosition = globalPos - mSelf->frameGeometry().topLeft();
            mSelf->setCursor(QCursor(Qt::SizeAllCursor));
        }
    } else {
        mSelf->releaseMouse();
    }
}

void Frameless::mouseMove(const QPoint &globalPos)
{
    QPoint gloPoint = globalPos;

    if (this->mLeftButtonPress && (this->mDir == Direction::None)) {
        if (mSelf->isMaximized()) {
            // mSelf->showNormal();

            // double xRatio = event->globalX() * 1.0 / mSelf->width();
            // double yRatio = event->globalY() * 1.0 / mSelf->height();
            // int widgetX = mSelf->width() * xRatio;
            // int widgetY = mSelf->height() * yRatio;

            // mSelf->move(event->globalX() - widgetX, event->globalY() - widgetY);
            // mSelf->layout()->setMargin(10);

            // dragPosition = event->globalPos() - mSelf->frameGeometry().topLeft();
            return;
        }

        mSelf->move(globalPos - this->mDragPosition);
        return;
    }

    if (this->mLeftButtonPress && mCanWindowResize) {
        gloPoint = calcFakeGlobalPos(gloPoint);
        mSelf->setGeometry(calcPositionRect(calcOriginRect(), gloPoint));
        return;
    }
}

void Frameless::mouseRelease()
{
    mSelf->setCursor(QCursor(Qt::ArrowCursor));
    this->mLeftButtonPress = false;
    this->mDir = Direction::None;
}

void Frameless::leave()
{
    if (this->mLeftButtonPress)
        return;

    this->mDir = Direction::None;
    mSelf->setCursor(QCursor(Qt::ArrowCursor));
}

void Frameless::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::HoverMove: {
        mouseHover(mSelf->mapToGlobal(static_cast<QHoverEvent *>(event)->pos()));
        event->accept();
    }
        break;
    case QEvent::MouseButtonPress: {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() != Qt::LeftButton)
            break;

        mousePress(mouseEvent->globalPos());
        mouseEvent->accept();
    }
        break;
    case QEvent::MouseButtonRelease: {
        mouseRelease();
    }
        break;
    case QEvent::MouseMove: {
        mouseMove(static_cast<QMouseEvent *>(event)->globalPos());
    }
        break;
    case QEvent::Leave: {
        leave();
    }
        break;
    case QEvent::FocusIn: {
        focusIn();
    }
        break;
    default:
        break;
    }
}
