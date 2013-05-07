/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QStatusBar>
#include <QWidget>
#include <QBoxLayout>
#include "medGuiExport.h"
#include "medMessageController.h"

class medStatusBarPrivate;

/**
* @class  medStatusBar
* @author Alexandre Abadie
* @brief  This is extended QStatusBar that can display custom messages.
*/

class MEDGUI_EXPORT medStatusBar : public QStatusBar
{
    Q_OBJECT
public:
    /**
     *  Constructor
     */
    medStatusBar ( QWidget * parent = 0 );
    /**
     * Destructor
     */
    ~medStatusBar();

    /**
     *  Set the status bar layout containing the widgets
     * (messages, quick access menu, quit button)
     *
     * @param : QBoxLayout * layout
     */
    void setStatusBarLayout ( QBoxLayout * layout );

    /**
     *  Set the status bar layout
     *
     * @return : QBoxLayout *
     */
    QBoxLayout * statusBarLayout();

    /**
     *  Set the available space on the statusBar
     *
     * @param : int space
     */
    void setAvailableSpace( int space );

    int getAvailableSpace();

    void init_availableSpace();

    void spaceManagement();

    void hideMessage( );

    void showHiddenMessage();

    void resizeEvent ( QResizeEvent* event );

public slots:
    /**
     * Add a widget containing a message (information, progress, error)
     * just after the quick access menu
     *
     * @param : QWidget * widget
     */
    void addMessage ( medMessage * message );

    /**
     *  Remove a widget (containing a message)
     */
    void removeMessage ( medMessage * message );


signals:
    void initializeAvailableSpace();

private:
    medStatusBarPrivate * d;
    int availableSpace; //available space on the status bar
    int statusBarWidth;
    QList<medMessage *> hiddenMessageList; // storing messages waiting to be displayed
    QList<medMessage *> messageList;    // storing displayed messages
};


