/* medViewContainerStack.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu May 13 12:37:58 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Thu May 13 12:39:00 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 3
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDVIEWCONTAINERSTACK_H
#define MEDVIEWCONTAINERSTACK_H

#include <QtGui>

class dtkAbstractView;
class medDataIndex;
class medViewContainer;
class medViewContainerStackPrivate;

class medViewContainerStack : public QStackedWidget
{
    Q_OBJECT

public:
     medViewContainerStack(QWidget *parent = 0);
    ~medViewContainerStack(void);

    medViewContainer *current(void);

    medViewContainer *single(void);
    medViewContainer *multi(void);
    medViewContainer *custom(void);
    medViewContainer *compare (void);
    medViewContainer *fuse (void);

signals:
    void dropped(const medDataIndex&);
    void focused(dtkAbstractView *);

private:
    medViewContainerStackPrivate *d;
};

#endif
