/* medWelcomeArea.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Oct  5 10:47:56 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Fri Mar 19 17:21:21 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 6
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDWELCOMEAREA_H
#define MEDWELCOMEAREA_H

#include <QtCore>
#include <QtGui>

class medWelcomeAreaPrivate;

class medWelcomeArea : public QWidget
{
    Q_OBJECT

public:
     medWelcomeArea(QWidget *parent = 0);
    ~medWelcomeArea(void);

    void setup(QStatusBar *status);
    void setdw(QStatusBar *status);

protected slots:
    void authenticate(void);

    void linkClicked(const QUrl &url);

private:
    medWelcomeAreaPrivate *d;
};

#endif // MEDWELCOMEAREA_H
