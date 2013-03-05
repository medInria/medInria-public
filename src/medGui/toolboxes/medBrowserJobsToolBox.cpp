/* medBrowserJobsToolBox.cpp ---
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

#include "medBrowserJobsToolBox.h"

#include <medProgressionStack.h>

#include <QtGui>

class medBrowserJobsToolBoxPrivate
{
public:
    medProgressionStack *stack;
};

medBrowserJobsToolBox::medBrowserJobsToolBox(QWidget *parent) : medToolBox(parent), d(new medBrowserJobsToolBoxPrivate)
{
    d->stack = new medProgressionStack(this);

    connect(d->stack, SIGNAL(shown()), this, SLOT(show()));
    connect(d->stack, SIGNAL(hidden()), this, SLOT(hide()));

    this->setTitle("Jobs");
    this->addWidget(d->stack);
}

medBrowserJobsToolBox::~medBrowserJobsToolBox()
{
    delete d;

    d = NULL;
}

medProgressionStack *medBrowserJobsToolBox::stack()
{
    return d->stack;
}
