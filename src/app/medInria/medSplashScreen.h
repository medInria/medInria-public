#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QtGui>
#include <QtWidgets>

class medSplashScreenPrivate;

/**
 * @brief create Splashcreen widget with translucent background.
 *
 * not based on QSplashScreen which does not support this feature.
 Almost all features from the QSplashscreen are replicated, including messages refreshed before a QApplication event loop is running.
 *
 */
class medSplashScreen : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief Creates a medSplashScreen.
     *
     * There are no parent widgets since it is started before the event loop.
     *
     * @param pixmap
    */
    medSplashScreen(const QPixmap& pixmap);
    ~medSplashScreen();

private:
    void paintEvent(QPaintEvent* pe);

    medSplashScreenPrivate * d;
};
