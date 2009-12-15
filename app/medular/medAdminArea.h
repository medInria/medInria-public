/* medAdminArea.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:36:33 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 09:36:37 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDADMINAREA_H
#define MEDADMINAREA_H

#include <QtGui/QWidget>

class medAdminAreaPrivate;

class medAdminArea : public QWidget
{
    Q_OBJECT

public:
     medAdminArea(QWidget *parent = 0);
    ~medAdminArea(void);

signals:
    void accepted(void);
    void rejected(void);

protected:
    void changeEvent(QEvent *e);

private:
    medAdminAreaPrivate *d;
};

#endif // MEDADMINAREA_H
