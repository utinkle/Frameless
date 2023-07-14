#ifndef FRAMELESSWIDGET_H
#define FRAMELESSWIDGET_H

#include <QWidget>

class Frameless;
class WarnMessageLabel;
class FramelessWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FramelessWidget(QWidget *parent = nullptr);
    int framelessBorder() const;

    void showPromptMsg(const QString &msg);

protected:
    void showEvent(QShowEvent *event) override;
    void changeEvent(QEvent *) override;
    bool event(QEvent *e) override;
    virtual bool withDropShadow();
    virtual bool canWindowMove() = 0;

    void setCanWindowResize(bool canResize);
    bool canWindowResize() const;

protected Q_SLOTS:
    void onWindowScreenChanged();
    void onScreenSizeChanged();

private:
    void initGraphicsEffect(QWidget *graphicsWidget);

private:
    QWindow             *m_window;
    QScreen             *m_screen;
    Frameless           *mFrameless;
    WarnMessageLabel    *mPromptLabel;
};

#endif // FRAMELESSWIDGET_H
