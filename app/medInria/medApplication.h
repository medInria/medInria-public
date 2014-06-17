/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtSingleApplication>
#include <QColor>

class medApplicationPrivate;
class medMainWindow;

// /////////////////////////////////////////////////////////////////
// medApplication
// /////////////////////////////////////////////////////////////////

/**
 * @brief Sets the style for the application and redirects some messages to logs or splash-screen.
 *
 */
class medApplication : public QtSingleApplication
{
    Q_OBJECT
    /**  Font color for messages redirected to the Splash screen.*/
    Q_PROPERTY(QColor msgColor READ msgColor WRITE setMsgColor)
    /** Text Alignment on the Splash screen.*/
    Q_PROPERTY(int msgAlignment READ msgAlignment WRITE setMsgAlignment)

public:
    /**
     * @brief
     *
     * @param argc
     * @param argv
    */
    medApplication(int & argc, char **argv);

    /**
     * @brief
     *
     */
    ~medApplication();

    /**
     * @brief Gets the font color for messages redirected to the Splash Screen.
     *
     */
    const QColor& msgColor();

    /**
     * @brief Sets the font color for messages redirected to the Splash Screen.
     *
     * @param color
     */
    void setMsgColor(const QColor& color);

    /**
     * @brief Gets the text alignment on the Splash screen.
     *
     */
    int msgAlignment();

    /**
     * @brief Sets the text alignment on the Splash screen.
     *
     * @param alignment
     */
    void setMsgAlignment(int alignment);
    
    bool event(QEvent *event);
    void setMainWindow(medMainWindow *mw);

signals:
    /**
     * @brief sends messages to the splash screen.
     *
     * @see QSplashScreen::showMessage
     *
     * @param message Text to display.
     * @param alignment Text alignment.
     * @param color Font color.
     */
    void showMessage(const QString& message,int alignment,const QColor& color);

public slots:
    /**
     * @brief Sends a signal intended for the splash screen to show a message.
     *
     * @see showMessage();
     *
     * @param message
     */
    void redirectMessageToSplash(const QString& message);

    /**
     * @brief Logs a message as a dtkTrace entry.
     *
     * @param message
     */
    void redirectMessageToLog(const QString & message);

    /**
     * @brief Logs an error message as a dtkError entry.
     *
     * @param message
     */
    void redirectErrorMessageToLog(const QString & message);

protected:
    /**
     * all initial registering should be done here
     */
    void registerToFactories();

private:
    medApplicationPrivate *d;
};


