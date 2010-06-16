/* medProgressionStack.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Jan 19 13:39:23 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Thu Mar 18 22:52:07 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 5
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDPROGRESSIONSTACK_H
#define MEDPROGRESSIONSTACK_H

#include "medGuiExport.h"

#include <QtGui/QWidget>

class medProgressionStackPrivate;

class MEDGUI_EXPORT medProgressionStack : public QWidget
{
    Q_OBJECT

public:
     medProgressionStack(QWidget *parent = 0);
    ~medProgressionStack(void);

    QSize sizeHint(void) const;

signals:
    void shown(void);
    void hidden(void);

public slots:
    void setLabel(QObject *sender, QString label);
    void setProgress(int progress);
    void removeItem();
private:
    medProgressionStackPrivate *d;
};

#endif // MEDPROGRESSIONSTACK_H
