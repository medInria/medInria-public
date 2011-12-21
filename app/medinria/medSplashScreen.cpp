#include "medSplashScreen.h"

////////////////////////////////////////////////////////////////////////////
medSplashScreen::medSplashScreen(const QPixmap& thePixmap)
 : QFrame(0, Qt::SplashScreen |Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint)
 , itsPixmap(thePixmap)
{
 setAttribute(Qt::WA_TranslucentBackground);
 setFixedSize(itsPixmap.size());
 QRect r(0, 0, itsPixmap.size().width(), itsPixmap.size().height());
 move(QApplication::desktop()->screenGeometry().center() - r.center());
 //repaint();
}

////////////////////////////////////////////////////////////////////////////
void medSplashScreen::clearMessage()
{
 itsMessage.clear();
 repaint();
}

////////////////////////////////////////////////////////////////////////////
void medSplashScreen::showMessage(const QString& theMessage, int theAlignment/* = Qt::AlignLeft*/, const QColor& theColor/* = Qt::black*/)
{
 itsMessage  = theMessage;
 itsAlignment = theAlignment;
 itsColor  = theColor;
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
 aPainter.drawPixmap(rect(), itsPixmap);
 aPainter.setPen(itsColor);
 aPainter.drawText(aTextRect, itsAlignment, itsMessage);
}
