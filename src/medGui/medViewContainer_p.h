/* medViewContainer_p.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Mar 17 14:44:12 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Mar 17 14:44:51 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 3
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDVIEWCONTAINER_P_H
#define MEDVIEWCONTAINER_P_H

#include <QtGui>

class dtkAbstractView;

class medViewContainerPrivate
{
public:
    QGridLayout *layout;

    dtkAbstractView *view;
};

#endif
