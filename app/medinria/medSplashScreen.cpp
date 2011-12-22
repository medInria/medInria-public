#include "medSplashScreen.h"


class medSplashScreenPrivate {
public:
    QPixmap  pixmap;
    QString  message;
    int   alignment;
    QColor  color;
};


////////////////////////////////////////////////////////////////////////////
medSplashScreen::medSplashScreen(const QPixmap& thePixmap)
    : QFrame(0, Qt::SplashScreen |Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint)
    , d(new medSplashScreenPrivate)
{
    d->pixmap = thePixmap;
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(d->pixmap.size());
    QRect r(0, 0, d->pixmap.size().width(), d->pixmap.size().height());
    move(QApplication::desktop()->screenGeometry().center() - r.center());
}


medSplashScreen::~medSplashScreen()
{
    delete d;
    d = NULL;
}

////////////////////////////////////////////////////////////////////////////
void medSplashScreen::clearMessage()
{
    d->message.clear();
    repaint();
}

////////////////////////////////////////////////////////////////////////////
void medSplashScreen::showMessage(const QString& theMessage,
                                  int theAlignment ,
                                  const QColor& theColor)
{
    d->message  = theMessage;
    d->alignment = theAlignment;
    d->color  = theColor;
    repaint();
}

void medSplashScreen::repaint()
{
    QWidget::repaint();
    QApplication::flush();
}


void medSplashScreen::finish(QWidget *mainWin)

{
    if (mainWin) {
#if defined(Q_WS_X11)
        extern void qt_x11_wait_for_window_manager(QWidget *mainWin);
        qt_x11_wait_for_window_manager(mainWin);
#endif
    }
    close();
}

////////////////////////////////////////////////////////////////////////////
void medSplashScreen::paintEvent(QPaintEvent* pe)
{
    QRect aTextRect(rect());
    aTextRect.setRect(aTextRect.x() + 5, aTextRect.y() + 5, aTextRect.width() - 10, aTextRect.height() - 10);

    QPainter aPainter(this);
    aPainter.drawPixmap(rect(), d->pixmap);
    aPainter.setPen(d->color);
    aPainter.drawText(aTextRect, d->alignment, d->message);
}
