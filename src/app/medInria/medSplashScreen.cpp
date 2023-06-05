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
    int   alignment;
    QColor  color;
    QRect aTextRect;
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

    // Compute once the right text rectangle
    d->aTextRect = rect();
    d->aTextRect.setRect(d->aTextRect.x() + 120,
                         d->aTextRect.y() + 5,
                         d->aTextRect.width() - 10,
                         d->aTextRect.height() - 10);
}

medSplashScreen::~medSplashScreen()
{
    delete d;
    d = nullptr;
}

////////////////////////////////////////////////////////////////////////////
void medSplashScreen::paintEvent(QPaintEvent* pe)
{
    Q_UNUSED(pe);

    QPainter aPainter(this);
    aPainter.drawPixmap(rect(), d->pixmap);
    aPainter.setPen(d->color);
}
