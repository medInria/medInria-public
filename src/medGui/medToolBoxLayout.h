/* medToolBoxLayout.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 09:02:09 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Fri Feb 19 10:03:42 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 6
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDTOOLBOXLAYOUT_H
#define MEDTOOLBOXLAYOUT_H

#include "medToolBox.h"

class medToolBoxLayoutPrivate;

class MEDGUI_EXPORT medToolBoxLayout : public medToolBox
{
    Q_OBJECT

public:
     medToolBoxLayout(QWidget *parent = 0);
    ~medToolBoxLayout(void);

signals:
    void modeChanged(int);
    void split(int, int);

private:
    medToolBoxLayoutPrivate *d;
};

#endif
