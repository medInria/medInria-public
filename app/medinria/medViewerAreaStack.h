/* medViewerAreaStack.h --- 
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

#ifndef MEDVIEWERAREASTACK_H
#define MEDVIEWERAREASTACK_H

#include <QtGui>

class dtkAbstractView;
class medDataIndex;
class medViewContainer;
class medViewerAreaStackPrivate;

class medViewerAreaStack : public QStackedWidget
{
    Q_OBJECT

public:
     medViewerAreaStack(QWidget *parent = 0);
    ~medViewerAreaStack(void);

    medViewContainer *current(void);

    medViewContainer *single(void);
    medViewContainer *multi(void);
    medViewContainer *custom(void);

    void setPatientId(int id);

signals:
    void dropped(const medDataIndex&);
    void focused(dtkAbstractView *);

private:
    medViewerAreaStackPrivate *d;
};

#endif
