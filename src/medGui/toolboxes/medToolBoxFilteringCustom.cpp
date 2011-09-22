/* medToolBoxFilteringCustom.cpp --- 
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

#include "medToolBoxFiltering.h"
#include "medToolBoxFilteringCustom.h"

class medToolBoxFilteringCustomPrivate
{
public:
    medToolBoxFiltering *parent;
};

medToolBoxFilteringCustom::medToolBoxFilteringCustom(QWidget *parent) : medToolBox(parent), d(new medToolBoxFilteringCustomPrivate)
{
    d->parent = NULL;
}

medToolBoxFilteringCustom::~medToolBoxFilteringCustom(void)
{
    delete d;

    d = NULL;
}

void medToolBoxFilteringCustom::setFilteringToolBox(medToolBoxFiltering *toolbox)
{
    d->parent = toolbox;
}

medToolBoxFiltering *medToolBoxFilteringCustom::parent(void)
{
    return d->parent;
}
