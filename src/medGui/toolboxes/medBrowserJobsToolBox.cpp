/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

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

medBrowserJobsToolBox::~medBrowserJobsToolBox(void)
{
    delete d;

    d = NULL;
}

medProgressionStack *medBrowserJobsToolBox::stack(void)
{
    return d->stack;
}
