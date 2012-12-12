/* medRegistrationAbstractToolBox.cpp ---
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

#include "medToolBoxRegistration.h"
#include "medRegistrationAbstractToolBox.h"

class medRegistrationAbstractToolBoxPrivate
{
public:
    medToolBoxRegistration *parent;
};

medRegistrationAbstractToolBox::medRegistrationAbstractToolBox(QWidget *parent) : medToolBox(parent), d(new medRegistrationAbstractToolBoxPrivate)
{
    d->parent = NULL;
}

medRegistrationAbstractToolBox::~medRegistrationAbstractToolBox(void)
{
    delete d;

    d = NULL;
}

void medRegistrationAbstractToolBox::setRegistrationToolBox(medToolBoxRegistration *toolbox)
{
    d->parent = toolbox;
}

medToolBoxRegistration *medRegistrationAbstractToolBox::parentToolBox(void)
{
    return d->parent;
}
