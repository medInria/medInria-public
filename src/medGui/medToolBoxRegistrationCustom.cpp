/* medToolBoxRegistrationCustom.cpp --- 
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
#include "medToolBoxRegistrationCustom.h"

class medToolBoxRegistrationCustomPrivate
{
public:
    medToolBoxRegistration *parent;
};

medToolBoxRegistrationCustom::medToolBoxRegistrationCustom(QWidget *parent) : medToolBox(parent), d(new medToolBoxRegistrationCustomPrivate)
{
    d->parent = NULL;
}

medToolBoxRegistrationCustom::~medToolBoxRegistrationCustom(void)
{
    delete d;

    d = NULL;
}

void medToolBoxRegistrationCustom::setRegistrationToolBox(medToolBoxRegistration *toolbox)
{
    d->parent = toolbox;
}

medToolBoxRegistration *medToolBoxRegistrationCustom::parent(void)
{
    return d->parent;
}
