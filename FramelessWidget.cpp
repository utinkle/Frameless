#include "FramelessWidget.h"
#include "AppGlobalInfo.h"
#include "Frameless.h"
#include "MachineHelper.h"
#include "gadgets/WarnMessageLabel.h"

#include <QScreen>
#include <QWindow>
#include <QDebug>
#include <QGraphicsEffect>
#include <QBoxLayout>

FramelessWidget::FramelessWidget(QWidget *parent)
    : QWidget(parent, Qt::Window)
    , m_window(nullptr)
    , m_screen(nullptr)
    , mFrameless(new Frameless(this, this))
    , mPromptLabel(new WarnMessageLabel(this))
{
}

int FramelessWidget::framelessBorder() const
{
    return mFrameless->framelessBorder();
}

void FramelessWidget::showPromptMsg(const QString &msg)
{
    mPromptLabel->showPromptMsg(msg);
}

void FramelessWidget::showEvent(QShowEvent *event)
{
    if (m_window)
        return QWidget::showEvent(event);

    if (QWindow *window = this->windowHandle()) {
        m_window = window;
        onWindowScreenChanged();

        QObject::connect(m_window, &QWindow::screenChanged, this, &FramelessWidget::onWindowScreenChanged, Qt::UniqueConnection);
    }

    return QWidget::showEvent(event);
}

void FramelessWidget::changeEvent(QEvent *e)
{
    const auto &layout = this->layout();
    auto enabledGraphics = layout && (layout->count() == 1) && withDropShadow();
    if (!enabledGraphics)
        return QWidget::changeEvent(e);

    QWidget *graphicsWidget = layout->itemAt(0)->widget();

    if (e->type() == QEvent::WindowStateChange) {
        if (windowState().testFlag(Qt::WindowMaximized)
                || windowState().testFlag(Qt::WindowFullScreen)) {
            layout->setMargin(0);

            if (auto effect = graphicsWidget->graphicsEffect()) {
                effect->deleteLater();
            }
        } else if (windowState().testFlag(Qt::WindowNoState)) {
            initGraphicsEffect(graphicsWidget);
        }
    }

    return QWidget::changeEvent(e);
}

bool FramelessWidget::event(QEvent *e)
{
    if (e->type() == QEvent::PolishRequest) {
        const auto &layout = this->layout();
        auto enabledGraphics = (layout->count() == 1) && withDropShadow();
        if (!enabledGraphics)
            return QWidget::event(e);

        QWidget *graphicsWidget = layout->itemAt(0)->widget();
        Q_ASSERT(graphicsWidget);

        if (graphicsWidget->graphicsEffect())
            return QWidget::event(e);

        layout->setMargin(10);
        initGraphicsEffect(graphicsWidget);
    } else {
        if (e->type() == QEvent::MouseButtonPress
                || e->type() == QEvent::MouseMove) {
            mFrameless->setCanWindowMove(canWindowMove());
        }

        mFrameless->targetEvent(e);
    }

    return QWidget::event(e);
}

bool FramelessWidget::withDropShadow()
{
    return MachineHelper::canUseCompositing();
}

void FramelessWidget::setCanWindowResize(bool canResize)
{
    mFrameless->setCanWindowResize(canResize);
}

bool FramelessWidget::canWindowResize() const
{
    return mFrameless->canWindowResize();
}

void FramelessWidget::onWindowScreenChanged()
{
    Q_ASSERT(m_window);
    QScreen *screen = m_window->screen();

    if (m_screen == screen)
        return;

    if (m_screen)
        disconnect(m_screen, &QScreen::availableGeometryChanged, this, &FramelessWidget::onScreenSizeChanged);

    m_screen = screen;
    connect(m_screen, &QScreen::availableGeometryChanged, this, &FramelessWidget::onScreenSizeChanged);
}

void FramelessWidget::onScreenSizeChanged()
{
    if (this->isWindow() && this->isMaximized()) {
        if (m_screen->availableGeometry() != this->geometry()) {
            this->setGeometry(m_screen->availableGeometry());
            this->setWindowState(this->windowState() | Qt::WindowMaximized);
        }
    }
}

void FramelessWidget::initGraphicsEffect(QWidget *graphicsWidget)
{
    Q_ASSERT(graphicsWidget);

    if (Q_LIKELY(MachineHelper::canUseCompositing())) {
        qobject_cast<QBoxLayout *>(this->layout())->setMargin(10);

        QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(this);
        effect->setBlurRadius(20);
        effect->setOffset(0);
        graphicsWidget->setGraphicsEffect(effect);
        return;
    }

    this->layout()->setMargin(5);

    if (graphicsWidget->graphicsEffect())
        graphicsWidget->graphicsEffect()->deleteLater();
}
