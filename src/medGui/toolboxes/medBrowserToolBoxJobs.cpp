/* medBrowserToolBoxJobs.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu May 13 15:20:03 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 11:16:43 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 17
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medBrowserToolBoxJobs.h"

#include <medProgressionStack.h>

#include <QtGui>

class medBrowserToolBoxJobsPrivate
{
public:
    medProgressionStack *stack;
};

medBrowserToolBoxJobs::medBrowserToolBoxJobs(QWidget *parent) : medToolBox(parent), d(new medBrowserToolBoxJobsPrivate)
{
    d->stack = new medProgressionStack(this);

    connect(d->stack, SIGNAL(shown()), this, SLOT(show()));
    connect(d->stack, SIGNAL(hidden()), this, SLOT(hide()));

    this->setTitle("Jobs");
    this->addWidget(d->stack);
}

medBrowserToolBoxJobs::~medBrowserToolBoxJobs(void)
{
    delete d;

    d = NULL;
}

medProgressionStack *medBrowserToolBoxJobs::stack(void)
{
    return d->stack;
}
