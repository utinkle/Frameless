#ifndef FRAMELESS_H
#define FRAMELESS_H

#include <QEvent>
#include <QMargins>
#include <QPoint>
#include <QRect>
#include <QString>

class QEvent;
class QWidget;
class QMouseEvent;
class QFocusEvent;
class FramelessWorker;
class Frameless
{
    friend class FramelessWorker;
public:
    Frameless(QWidget *self);

protected:
    enum class Direction {
        None = -1,
        Up,
        Down,
        Left,
        Right,
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight
    };

    void setDirection(Direction dir);
    Direction direction() const;

    virtual bool canWindowMove() = 0;

    void setCanWindowResize(bool canResize);
    bool canWindowResize() const;

    void setDragPosition(const QPoint &dragPosition);
    QPoint dragPosition() const;

    void setLeftMouseButtonPressed(bool pressed);
    bool leftMouseButtonPressed() const;

    void event(QEvent *event);

private:
    // worker and target
    QWidget *           mSelf;
    FramelessWorker *   mWorker;
    bool                mCanWindowResize;

    // state for window
    QPoint              mDragPosition;
    Direction           mDir;
    bool                mLeftButtonPress;
};

#endif // FRAMELESS_H
