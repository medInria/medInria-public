/* medToolBoxJobs.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu May 13 15:20:03 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Thu May 13 16:03:34 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 13
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medProgressionStack.h"
#include "medToolBoxJobs.h"

#include <QtGui>

class medToolBoxJobsPrivate
{
public:
    medProgressionStack *stack;
};

medToolBoxJobs::medToolBoxJobs(QWidget *parent) : medToolBox(parent), d(new medToolBoxJobsPrivate)
{
    d->stack = new medProgressionStack(this);

    connect(d->stack, SIGNAL(shown()), this, SLOT(show()));
    connect(d->stack, SIGNAL(hidden()), this, SLOT(hide()));

    this->setTitle("Jobs");
    this->setWidget(d->stack);
}

medToolBoxJobs::~medToolBoxJobs(void)
{
    delete d;

    d = NULL;
}

medProgressionStack *medToolBoxJobs::stack(void)
{
    return d->stack;
}
