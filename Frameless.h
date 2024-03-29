#ifndef FRAMELESS_H
#define FRAMELESS_H

#include <QEvent>
#include <QMargins>
#include <QObject>
#include <QPoint>
#include <QRect>
#include <QString>

class QEvent;
class QWidget;
class QMouseEvent;
class QFocusEvent;
class FramelessWorker;
class Frameless : public QObject
{
    Q_OBJECT

public:
    explicit Frameless(QWidget *self, QObject *parent = nullptr);

    void setCanWindowMove(bool canMove);
    bool canWindowMove() const;

    void setCanWindowResize(bool canResize);
    bool canWindowResize() const;

    int framelessBorder() const;

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

    void setCurrentCanWindowMove(bool canWindowMove);
    bool currentCanWindowMove() const;

    void setDragPosition(const QPoint &dragPosition);
    QPoint dragPosition() const;

    void setLeftMouseButtonPressed(bool pressed);
    bool leftMouseButtonPressed() const;

    void setAcceptSystemResize(bool accept);
    bool acceptSystemResize() const;

    void setAcceptSystemMoving(bool accept);
    bool acceptSystemMoving() const;

    bool framelessMoving() const;

    void targetEvent(QEvent *event);

    Q_INVOKABLE void moveByFrameless(const QPoint &pos);
    Q_INVOKABLE void setGeometryByFrameless(const QRect &rect);
    Q_INVOKABLE void setCursorByFrameless(int shape);
    Q_INVOKABLE void unsetCursorByFrameless();
    Q_INVOKABLE void readyToStartMove(int shape);
    Q_INVOKABLE void accpetSystemResize();

private:
    bool startSystemResize(QWidget *window, const QPoint &, int dir);
    bool startSystemMove(QWidget *window, const QPoint &);
    void deactivateWindowWhenSystemMove(QWidget *window);

private:
    // worker and target
    QWidget *           mSelf;
    FramelessWorker *   mWorker;
    bool                mCanWindowMove = false;
    bool                mCanWindowResize = false;

    // state for window
    QPoint              mDragPosition;
    Direction           mDir;
    bool                mLeftButtonPress = false;
    bool                mAlreadyChangeCursor = false;
    bool                mCurrentCanWindowMove = false;
    int                 mOverrideCursorShape = false;
    bool                mAcceptSystemResize = false;
    bool                mAcceptSystemMoving = false;
};

#endif // FRAMELESS_H
