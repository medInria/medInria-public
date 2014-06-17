/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtGui>

class medSplashScreenPrivate;


/**
 * @brief create SplaScreen widget with translucent background.
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

    /**
     * @brief hides the splashscreen when w is shown.
     *
     * @param w
     */
    void finish(QWidget *w);

    /**
     * @brief Repaints the widget without the need of an event loop;
     *
     */
    void repaint();

public slots:
    /**
     * @brief Clears the message.
     *
     */
    void clearMessage();

    /**
     * @brief Shows a message with the desired alignment and color.
     *
     * @param theMessage
     * @param theAlignment
     * @param theColor
     */
    void showMessage(const QString& theMessage,
                     int theAlignment = Qt::AlignLeft,
                     const QColor& theColor = Qt::black);

private:
    /**
     * @brief
     *
     * @param pe
     */
    void paintEvent(QPaintEvent* pe);

    medSplashScreenPrivate * d;
};

