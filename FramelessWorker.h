#ifndef FRAMELESSWORKER_H
#define FRAMELESSWORKER_H

#include <QMutex>
#include <QThread>
#include <QWaitCondition>

class FramelessEvent;
class FramelessLeaveEvent;
class FramelessFocusInEvent;
class FramelessMouseMoveEvent;
class FramelessMousePressEvent;
class FramelessMouseHoverEvent;
class FramelessMouseReleaseEvent;
class FramelessWorker : public QThread
{
    Q_OBJECT
public:
    static FramelessWorker *instance();
    void postEvent(FramelessEvent *event);

public Q_SLOTS:
    void exit();

protected:
    FramelessEvent *takeEvent();
    void run() override;

    void focusIn(FramelessFocusInEvent *event);
    void mouseHover(FramelessMouseHoverEvent *event);
    void mousePress(FramelessMousePressEvent *event);
    void mouseMove(FramelessMouseMoveEvent *event);
    void mouseRelease(FramelessMouseReleaseEvent *event);
    void leave(FramelessLeaveEvent *event);

    struct DirAndCursorShape
    {
        int dir = -1;
        Qt::CursorShape cursorShape = Qt::ArrowCursor;
    };
    static DirAndCursorShape calcDirAndCursorShape(const QRect &rOrigin, const QPoint &cursorGlobalPoint, int framelessBorder);
    static QRect calcPositionRect(int dir, QWidget *target, const QRect &rOrigin, const QPoint &gloPoint);
    static QRect calcOriginRect(QWidget *taget);
    static QPoint calcFakeGlobalPos(QWidget *taget, const QPoint &gloPoint);

private:
    explicit FramelessWorker(QObject *parent = nullptr);

private:
    static FramelessWorker *    mInstance;
    bool                        mExit = false;
    QList<FramelessEvent *>     mEventQueue;
    QWaitCondition              mCondition;
    QMutex                      mMutex;
};

#endif // FRAMELESSWORKER_H
