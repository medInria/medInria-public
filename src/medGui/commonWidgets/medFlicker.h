/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtCore>
#include <QtGui>

class medFlickerPrivate;

class medFlicker: public QObject
{
    Q_OBJECT

public:
     medFlicker(QObject *parent = 0);
    ~medFlicker();

    void activateOn(QWidget *widget);
    void deactivateFrom(QWidget *widget);

    bool eventFilter(QObject *object, QEvent *event);

protected:
    void timerEvent(QTimerEvent *event);

private:
    medFlickerPrivate *d;
};



// /////////////////////////////////////////////////////////////////
// Credits
// /////////////////////////////////////////////////////////////////

// Copyright (C) 2008-2008 Trolltech ASA. All rights reserved.
