/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QStatusBar>
#include <QWidget>
#include <QBoxLayout>
#include <medMessageController.h>

#include <medCoreLegacyExport.h>

class medStatusBarPrivate;

/**
* @class  medStatusBar
* @author Alexandre Abadie, Florent Leray
* @brief  This is extended QStatusBar that can display custom messages.
*/
class MEDCORELEGACY_EXPORT medStatusBar : public QStatusBar
{
    Q_OBJECT
public:

    medStatusBar ( QWidget * parent = nullptr );
    ~medStatusBar();

    void setStatusBarLayout ( QBoxLayout * layout );
    QBoxLayout * statusBarLayout();

    void setAvailableSpace( int space );
    int getAvailableSpace();

    void init_availableSpace();

    void spaceManagement();

    void hideMessage( );
    void showHiddenMessage();

    void resizeEvent ( QResizeEvent* event );

public slots:
    void addMessage ( medMessage * message );
    void removeMessage ( medMessage * message );

signals:
    void initializeAvailableSpace();

private:
    medStatusBarPrivate * d;
};


