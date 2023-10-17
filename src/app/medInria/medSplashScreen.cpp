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


medSplashScreen::medSplashScreen(const QPixmap& thePixmap)
    : QWidget(0, Qt::SplashScreen |Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint)
{
    QPixmap pixmap = thePixmap;
    setFixedSize(pixmap.size());
    QRect r(0, 0, pixmap.size().width(), pixmap.size().height());

    // Get back the previous screen used to display the application
    int currentScreen = 0;
    QVariant currentScreenQV = medSettingsManager::instance().value("medMainWindow", "currentScreen");
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
