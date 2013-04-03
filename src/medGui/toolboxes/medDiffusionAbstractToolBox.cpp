/* medDiffusionAbstractToolBox.cpp ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Nov 10 15:42:09 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 15:58:03 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include "medDiffusionSelectorToolBox.h"

#include "medDiffusionAbstractToolBox.h"
#include "medDiffusionAbstractToolBox_p.h"

#include <dtkCore/dtkAbstractObject.h>

medDiffusionAbstractToolBox::medDiffusionAbstractToolBox(QWidget *parent) : medToolBox(parent), d(new medDiffusionAbstractToolBoxPrivate)
{
    d->parent  = NULL;
}

medDiffusionAbstractToolBox::~medDiffusionAbstractToolBox(void)
{
    delete d;

    d = NULL;
}

void medDiffusionAbstractToolBox::setDiffusionToolBox(medDiffusionSelectorToolBox *toolBox)
{
    d->parent = toolBox;
}

medDiffusionSelectorToolBox *medDiffusionAbstractToolBox::parentToolBox(void)
{
    return d->parent;
}


dtkAbstractData *medDiffusionAbstractToolBox::output() const
{
    DTK_DEFAULT_IMPLEMENTATION;
    return NULL;
}
