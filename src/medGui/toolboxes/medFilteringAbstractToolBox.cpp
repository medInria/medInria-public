/* medFilteringAbstractToolBox.cpp ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Nov 10 15:42:09 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 15:58:03 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 17
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include "medFilteringSelectorToolBox.h"
#include "medFilteringAbstractToolBox.h"

class medFilteringAbstractToolBoxPrivate
{
public:
    medFilteringSelectorToolBox *parent;
};

medFilteringAbstractToolBox::medFilteringAbstractToolBox(QWidget *parent) : medToolBox(parent), d(new medFilteringAbstractToolBoxPrivate)
{
    d->parent = NULL;
}

medFilteringAbstractToolBox::~medFilteringAbstractToolBox()
{
    delete d;

    d = NULL;
}

void medFilteringAbstractToolBox::setFilteringToolBox(medFilteringSelectorToolBox *toolbox)
{
    d->parent = toolbox;
}

medFilteringSelectorToolBox *medFilteringAbstractToolBox::parentToolBox()
{
    return d->parent;
}
