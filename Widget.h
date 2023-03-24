#ifndef WIDGET_H
#define WIDGET_H

#include "FramelessWidget.h"

#include <QWidget>

class Widget : public FramelessWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    inline bool canWindowMove() override {
        return true;
    }
};
#endif // WIDGET_H
