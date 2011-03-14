/* medToolBoxDiffusionCustom.cpp ---
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

#include "medToolBoxDiffusion.h"

#include "medToolBoxDiffusionCustom.h"
#include "medToolBoxDiffusionCustom_p.h"

#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractProcessFactory.h>

medToolBoxDiffusionCustom::medToolBoxDiffusionCustom(QWidget *parent) : medToolBox(parent), d(new medToolBoxDiffusionCustomPrivate)
{
    d->parent  = NULL;
    d->process = NULL;
}

medToolBoxDiffusionCustom::~medToolBoxDiffusionCustom(void)
{
    delete d;

    d = NULL;
}

void medToolBoxDiffusionCustom::setDiffusionToolBox(medToolBoxDiffusion *toolbox)
{
    d->parent = toolbox;
}

medToolBoxDiffusion *medToolBoxDiffusionCustom::parent(void)
{
    return d->parent;
}

void medToolBoxDiffusionCustom::setProcess (dtkAbstractProcess *process)
{
    d->process = process;
}

dtkAbstractProcess *medToolBoxDiffusionCustom::process (void) const
{
    return d->process;
}

dtkAbstractData *medToolBoxDiffusionCustom::output (void) const
{
    DTK_DEFAULT_IMPLEMENTATION;
    return NULL;
}
