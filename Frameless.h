#ifndef FRAMELESS_H
#define FRAMELESS_H

#include <QEvent>
#include <QMargins>
#include <QPoint>
#include <QRect>
#include <QString>

class QWidget;
class QEvent;
class QMouseEvent;
class QFocusEvent;
class Frameless
{
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

    virtual bool canWindowMove() = 0;

    void setCanWindowResize(bool canResize);
    bool canWindowResize() const;

    void focusIn();
    void mouseHover(const QPoint &globalPos);
    void mousePress(const QPoint &globalPos);
    void mouseMove(const QPoint &globalPos);
    void mouseRelease();
    void leave();
    void event(QEvent *event);

    void calcDirAndCursorShape(const QRect &rOrigin, const QPoint &cursorGlobalPoint);
    QRect calcPositionRect(const QRect &rOrigin, const QPoint &gloPoint);
    QRect calcOriginRect();
    QPoint calcFakeGlobalPos(const QPoint &gloPoint);

private:
    QWidget *   mSelf;
    QPoint      mPoint;
    QPoint      mDragPosition;
    Direction   mDir;
    bool        mLeftButtonPress;
    bool        mCanWindowResize;
};

#endif // FRAMELESS_H
