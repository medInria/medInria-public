/* medViewContainer_p.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Mar 17 14:44:12 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Jun 15 16:20:24 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 5
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
class medViewContainer;
class medViewPool;

class medViewContainerPrivate
{
public:
    QGridLayout *layout;

    dtkAbstractView *view;

    medViewContainer *current;

    // for view synchronization
    /*
      QList<dtkAbstractView *> views;
      dtkAbstractView         *refView;
      int                      synchronize;
    */
    medViewPool *pool;

    QHash<QString, QString> viewProperties;
};

#endif
