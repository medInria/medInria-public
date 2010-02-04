/* medProgressionStack.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Jan 19 13:39:23 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Jan 19 13:39:39 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 2
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDPROGRESSIONSTACK_H
#define MEDPROGRESSIONSTACK_H

#include <QtGui/QWidget>

class medProgressionStackPrivate;

class medProgressionStack : public QWidget
{
    Q_OBJECT

public:
     medProgressionStack(QWidget *parent = 0);
    ~medProgressionStack(void);

    QSize sizeHint(void) const;

public slots:
    void setProgress(int progress);

private:
    medProgressionStackPrivate *d;
};

#endif // MEDPROGRESSIONSTACK_H
