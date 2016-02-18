/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSplashScreen.h>
#include <dtkCore/dtkPlugin.h>
#include <medPluginManager.h>

class medSplashScreenPrivate {
public:
    QPixmap  pixmap;
    QString  message;
    int   alignment;
    QColor  color;
};


////////////////////////////////////////////////////////////////////////////
medSplashScreen::medSplashScreen(const QPixmap& thePixmap)
    : QWidget(0, Qt::SplashScreen |Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint)
    , d(new medSplashScreenPrivate)
{
    d->pixmap = thePixmap;
    d->color = Qt::white; // TODO get from QSS
    d->alignment = Qt::AlignBottom|Qt::AlignLeft;
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
void medSplashScreen::showMessage(const QString& message)
{
    const dtkPlugin* plugin = medPluginManager::instance()->plugin(message);
    if (plugin)
    {
        d->message = plugin->description();
        //in case descriptions have several lines (html or plain text)
        d->message = d->message.section('\n',0,0);
        d->message = d->message.section("<br/>",0,0);
    }
    else
        d->message = message;

    repaint();
}

void medSplashScreen::repaint()
{
    QWidget::repaint();
    QApplication::flush();
    qApp->processEvents(QEventLoop::AllEvents);
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
    QPoint aTextPos(rect().width()*0.25, rect().height()*0.85);

    QPainter aPainter(this);
    aPainter.drawPixmap(rect(), d->pixmap);
    aPainter.setPen(d->color);
    aPainter.drawText(aTextPos, d->message);
}
