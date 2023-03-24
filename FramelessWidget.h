#ifndef FRAMELESSWIDGET_H
#define FRAMELESSWIDGET_H

#include <QWidget>

class Frameless;
class FramelessWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FramelessWidget(QWidget *parent = nullptr);
    int framelessBorder() const;

protected:
    bool event(QEvent *e) override;
    virtual bool canWindowMove() = 0;

    void setCanWindowResize(bool canResize);
    bool canWindowResize() const;

private:
    Frameless   *mFrameless;
};

#endif // FRAMELESSWIDGET_H
