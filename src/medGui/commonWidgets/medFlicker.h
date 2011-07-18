/* medFlicker.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 18:16:43 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Fri Feb 19 18:22:17 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 4
 */

/* Commentary: 
 *
 * See credits at EOF.
 */

/* Change log:
 * 
 */

#ifndef MEDFLICKER_H
#define MEDFLICKER_H

#include <QtCore>
#include <QtGui>

class medFlickerPrivate;

class medFlicker: public QObject
{
    Q_OBJECT

public:
     medFlicker(QObject *parent = 0);
    ~medFlicker(void);

    void activateOn(QWidget *widget);
    void deactivateFrom(QWidget *widget);

    bool eventFilter(QObject *object, QEvent *event);

protected:
    void timerEvent(QTimerEvent *event);

private:
    medFlickerPrivate *d;
};

#endif

// /////////////////////////////////////////////////////////////////
// Credits
// /////////////////////////////////////////////////////////////////

// Copyright (C) 2008-2008 Trolltech ASA. All rights reserved.
