#ifndef FRAMELESSWORKEREVENT_H
#define FRAMELESSWORKEREVENT_H

#include <QPoint>

class Frameless;
class QWidget;
struct FramelessEvent
{
    enum EventType {
        UnkonwEvent = -1,
        FocusIn,
        MouseHover,
        MousePress,
        MouseMove,
        MouseRelease,
        Leave,
    };

    FramelessEvent(EventType type);
    virtual ~FramelessEvent();

    EventType type();
    QWidget *target = nullptr;
    Frameless *frameless = nullptr;

private:
    EventType mEventType = EventType::UnkonwEvent;
};

struct FramelessFocusInEvent : public FramelessEvent
{
    FramelessFocusInEvent();

    bool canWindowResize = true;
};

struct FramelessMouseHoverEvent : public FramelessEvent
{
    FramelessMouseHoverEvent();

    QPoint globalCursorPositon;
};

struct FramelessMousePressEvent : public FramelessEvent
{
    FramelessMousePressEvent();

    QPoint globalCursorPositon;
    bool canWindowMove = true;
};

struct FramelessMouseMoveEvent : public FramelessEvent
{
    FramelessMouseMoveEvent();

    QPoint globalCursorPositon;
    bool canWindowResize = true;
};

struct FramelessMouseReleaseEvent : public FramelessEvent
{
    FramelessMouseReleaseEvent();
};

struct FramelessLeaveEvent : public FramelessEvent
{
    FramelessLeaveEvent();
};
#endif // FRAMELESSWORKEREVENT_H
