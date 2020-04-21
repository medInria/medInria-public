/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QTest>

#include <medSplashScreen.h>
#include <dtkCoreSupport/dtkPlugin.h>
#include <medPluginManager.h>
#include <medSettingsManager.h>

class medSplashScreenPrivate
{
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
    // Graphic
    d->pixmap = thePixmap;
    d->color = Qt::white;

    // Geometry
    d->alignment = Qt::AlignBottom|Qt::AlignLeft;
    setFixedSize(d->pixmap.size());

    QRect r(0, 0, d->pixmap.size().width(), d->pixmap.size().height());

    // Get back the previous screen used to display the application
    medSettingsManager *manager = medSettingsManager::instance();
    int currentScreen = 0;

    QVariant currentScreenQV = manager->value("medMainWindow", "currentScreen");
    if (!currentScreenQV.isNull())
    {
        currentScreen = currentScreenQV.toInt();

        // If the previous used screen has been removed, initialization
        if (currentScreen >= QApplication::desktop()->screenCount())
        {
            currentScreen = 0;
        }
    }

    // Move the Splash screen at the center of the chosen screen
    move(QApplication::desktop()->screenGeometry(currentScreen).center() - r.center());
}

medSplashScreen::~medSplashScreen()
{
    delete d;
    d = nullptr;
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
        d->message = QString("Loading: ") + plugin->name();
    }
    else
    {
        d->message = QString("Loading: ") + message;
    }

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
    if (mainWin)
    {
#if defined(Q_OS_LINUX)
        QTest::qWaitForWindowExposed(mainWin);
#endif
    }
    close();
}

////////////////////////////////////////////////////////////////////////////
void medSplashScreen::paintEvent(QPaintEvent* pe)
{
    QRect aTextRect(rect());
    aTextRect.setRect(aTextRect.x() + 120,
                      aTextRect.y() + 5,
                      aTextRect.width() - 10,
                      aTextRect.height() - 10);

    QPainter aPainter(this);
    aPainter.drawPixmap(rect(), d->pixmap);
    aPainter.setPen(d->color);
    aPainter.drawText(aTextRect, d->alignment, d->message);
}
