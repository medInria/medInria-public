/* medToolBoxFactory.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Nov 10 14:33:05 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 14:51:04 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 29
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medToolBox.h"
#include "medToolBoxFactory.h"

class medToolBoxFactoryPrivate
{
public:
    medToolBoxFactory::medToolBoxRegistrationCustomCreatorHash custom_registration_creators;
};

medToolBoxFactory *medToolBoxFactory::instance(void)
{
    if(!s_instance)
        s_instance = new medToolBoxFactory;

    return s_instance;
}

bool medToolBoxFactory::registerCustomRegistrationToolBox(QString type, medToolBoxRegistrationCustomCreator func)
{
    if(!d->custom_registration_creators.contains(type)) {
        d->custom_registration_creators.insert(type, func);
        return true;
    }

    return false;
}

medToolBoxRegistrationCustom *medToolBoxFactory::createCustomRegistrationToolBox(QString type)
{
    if(!d->custom_registration_creators.contains(type))
        return NULL;

    medToolBoxRegistrationCustom *toolbox = d->custom_registration_creators[type]();

    return toolbox;
}

medToolBoxFactory::medToolBoxFactory(void) : dtkAbstractFactory(), d(new medToolBoxFactoryPrivate)
{

}

medToolBoxFactory::~medToolBoxFactory(void)
{
    delete d;

    d = NULL;
}

medToolBoxFactory *medToolBoxFactory::s_instance = NULL;
