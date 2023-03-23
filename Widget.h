#ifndef WIDGET_H
#define WIDGET_H

#include "Frameless.h"

#include <QWidget>

class Widget : public QWidget, public Frameless
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    inline bool canWindowMove() override {
        return true;
    }

    bool event(QEvent *event) override;
};
#endif // WIDGET_H
