#include "Frameless.h"
#include "FramelessWorker.h"
#include "FramelessWorkerEvent.h"
#include "AppGlobalInfo.h"
#include "MachineHelper.h"

#include <QWidget>
#include <QWindow>
#include <QMouseEvent>
#include <QRect>
#include <QDebug>
#include <QApplication>
#include <QTimer>

#ifdef Q_OS_WINDOWS
#include <dwmapi.h>
#else
#endif

#define FREMELESS_BORDER 6

Frameless::Frameless(QWidget *self, QObject *parent)
    : QObject(parent)
    , mSelf(self)
    , mWorker(FramelessWorker::instance())
    , mCanWindowMove(false)
    , mCanWindowResize(false)
    , mDir(Direction::None)
    , mLeftButtonPress(false)
    , mAlreadyChangeCursor(false)
    , mCurrentCanWindowMove(false)
    , mOverrideCursorShape(Qt::ArrowCursor)
{
    mSelf->setWindowFlags(mSelf->windowFlags() | Qt::FramelessWindowHint);
    mSelf->setAttribute(Qt::WA_TranslucentBackground, MachineHelper::canUseCompositing());
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

void Frameless::setCurrentCanWindowMove(bool canWindowMove)
{
    mCurrentCanWindowMove = canWindowMove;
}

bool Frameless::currentCanWindowMove() const
{
    return mCurrentCanWindowMove;
}

void Frameless::setCanWindowMove(bool canMove)
{
    qInfo() << "+++!!!000111222333" << canMove;
    mCanWindowMove = canMove;
}

bool Frameless::canWindowMove() const
{
    return mCanWindowMove;
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

int Frameless::framelessBorder() const
{
    return FREMELESS_BORDER;
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

void Frameless::setAcceptSystemResize(bool accept)
{
    mAcceptSystemResize = accept;
}

bool Frameless::acceptSystemResize() const
{
    return mAcceptSystemResize;
}

void Frameless::setAcceptSystemMoving(bool accept)
{
    mAcceptSystemMoving = accept;
}

bool Frameless::acceptSystemMoving() const
{
    return mAcceptSystemMoving;
}

void Frameless::targetEvent(QEvent *event)
{
    FramelessEvent *framelessEvent = nullptr;
    switch (event->type()) {
    case QEvent::HoverMove: {
        FramelessMouseHoverEvent *mouseHoverEvent = new FramelessMouseHoverEvent();
        mouseHoverEvent->globalCursorPositon = mSelf->mapToGlobal(static_cast<QHoverEvent *>(event)->pos());
        mouseHoverEvent->canWindowResize = mCanWindowResize;
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
        qInfo() << "targetEvent............." << mCanWindowMove;
        mousePressEvent->canWindowMove = mCanWindowMove;
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
    case QEvent::WindowDeactivate: {
        deactivateWindowWhenSystemMove(mSelf);
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

void Frameless::moveByFrameless(const QPoint &pos)
{
    mSelf->move(pos);
}

void Frameless::setGeometryByFrameless(const QRect &rect)
{
    mSelf->setGeometry(rect);
}

void Frameless::setCursorByFrameless(int shape)
{
    if (mAlreadyChangeCursor) {
        if (shape == Qt::ArrowCursor) {
            unsetCursorByFrameless();
            return;
        }

        qApp->changeOverrideCursor(Qt::CursorShape(shape));
    } else {
        if (shape == Qt::ArrowCursor)
            return;

        qApp->setOverrideCursor(Qt::CursorShape(shape));
        mAlreadyChangeCursor = true;
    }

    mOverrideCursorShape = shape;
}

void Frameless::unsetCursorByFrameless()
{
    if (!mAlreadyChangeCursor)
        return;

    mAlreadyChangeCursor = false;

    QCursor *cursor = qApp->overrideCursor();
    if (!cursor || cursor->shape() != mOverrideCursorShape)
        return;

    qApp->restoreOverrideCursor();
}


void Frameless::readyToStartMove(int shape)
{
    setAcceptSystemMoving(startSystemMove(mSelf, {0, 0}));
    setCursorByFrameless(shape);
}

void Frameless::accpetSystemResize()
{
    setAcceptSystemResize(startSystemResize(mSelf, {0, 0}, static_cast<int>(direction())));
}

#ifdef Q_OS_WIN
static inline DWORD directionToWinOrientation(int dir)
{
    switch (static_cast<Frameless::Direction>(dir)) {
    case Frameless::Direction::Left:
        return 0xf001; // SZ_SIZELEFT;
    case Frameless::Direction::Right:
        return 0xf002; // SZ_SIZERIGHT;
    case Frameless::Direction::Up:
        return 0xf003; // SZ_SIZETOP;
    case Frameless::Direction::TopLeft:
        return 0xf004; // SZ_SIZETOPLEFT;
    case Frameless::Direction::TopRight:
        return 0xf005; // SZ_SIZETOPRIGHT
    case Frameless::Direction::Down:
        return 0xf006; // SZ_SIZEBOTTOM;
    case Frameless::Direction::BottomLeft:
        return 0xf007; // SZ_SIZEBOTTOMLEFT
    case Frameless::Direction::BottomRight:
        return 0xf008; // SZ_SIZEBOTTOMRIGHT
    default:
        break;
    }

    return 0;
}
#endif

bool Frameless::startSystemResize(QWidget *window, const QPoint &, int dir)
{
    bool supportWindowResize = window->isWindow()
            && window->windowHandle()
            && !(window->windowFlags() & Qt::X11BypassWindowManagerHint)
            && !window->testAttribute(Qt::WA_DontShowOnScreen)
            && !window->hasHeightForWidth();

    if (!supportWindowResize)
        return false;

#ifdef Q_OS_WIN
    HWND hwnd = reinterpret_cast<HWND>(window->windowHandle()->winId());
    if (!GetSystemMenu(hwnd, FALSE))
        return false;

    ReleaseCapture();
    DWORD ori = directionToWinOrientation(dir);
    PostMessage(hwnd, WM_SYSCOMMAND, ori, 0);
    return true;
#else
    return false;
#endif
}

bool Frameless::startSystemMove(QWidget *window, const QPoint &)
{
    bool supportWindowResize = window->isWindow()
            && window->windowHandle()
            && !(window->windowFlags() & Qt::X11BypassWindowManagerHint)
            && !window->testAttribute(Qt::WA_DontShowOnScreen);

    if (!supportWindowResize)
        return false;

#ifdef Q_OS_WIN
    HWND hwnd = reinterpret_cast<HWND>(window->windowHandle()->winId());
    if (!GetSystemMenu(hwnd, FALSE))
        return false;

    ReleaseCapture();
    PostMessage(hwnd, WM_SYSCOMMAND, 0xF012 /*SC_DRAGMOVE*/, 0);
    QTimer::singleShot(0, this, [=]() {
        SHORT leftButtonState = GetAsyncKeyState(VK_LBUTTON);
        if (leftButtonState & 0x8000)
            return;

        PostMessage(hwnd, WM_LBUTTONUP, 0, 0);
    });
    return true;
#else
    return false;
#endif
}

void Frameless::deactivateWindowWhenSystemMove(QWidget *window)
{
    HWND hwnd = reinterpret_cast<HWND>(window->windowHandle()->winId());
#ifdef Q_OS_WINDOWS
    if (acceptSystemMoving() && leftMouseButtonPressed()) {
        PostMessage(hwnd, WM_LBUTTONUP, 0, 0);
        PostMessage(hwnd, WM_WINDOWPOSCHANGED, 0, 0);
    }
#endif
}
