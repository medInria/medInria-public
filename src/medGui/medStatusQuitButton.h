/* medStatusQuitButton.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu Oct  7 11:36:25 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Thu Oct  7 12:32:17 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 12
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDSTATUSQUITBUTTON_H
#define MEDSTATUSQUITBUTTON_H

#include <QtGui/QWidget>

class medStatusQuitButton : public QWidget
{
    Q_OBJECT

public:
     medStatusQuitButton(QWidget *parent = NULL);
    ~medStatusQuitButton(void);

    QSize sizeHint(void) const;

signals:
    void quit(void);

protected:
    void mousePressEvent(QMouseEvent *event);
};

#endif
