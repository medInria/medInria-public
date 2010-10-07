/* medAbstractPacsFactory.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Nov  7 15:54:10 2008 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Oct  6 13:09:06 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 56
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medAbstractPacsFactory.h"
#include "medAbstractPacsEchoScu.h"
#include "medAbstractPacsFindScu.h"
#include "medAbstractPacsMoveScu.h"
#include "medAbstractPacsStoreScu.h"
#include "medAbstractPacsStoreScp.h"

class medAbstractPacsFactoryPrivate
{
public:
    medAbstractPacsFactory::medAbstractPacsEchoScuCreatorHash echo_scu_creators;
    medAbstractPacsFactory::medAbstractPacsFindScuCreatorHash find_scu_creators;
    medAbstractPacsFactory::medAbstractPacsMoveScuCreatorHash move_scu_creators;
    medAbstractPacsFactory::medAbstractPacsStoreScuCreatorHash store_scu_creators; 
    medAbstractPacsFactory::medAbstractPacsStoreScpCreatorHash store_scp_creators;


};

medAbstractPacsFactory *medAbstractPacsFactory::instance(void)
{
    if(!s_instance)
        s_instance = new medAbstractPacsFactory;

    return s_instance;
}

medAbstractPacsEchoScu * medAbstractPacsFactory::createEchoScu( QString type )
{
    if(!d->echo_scu_creators.contains(type))
        return NULL;

    medAbstractPacsEchoScu *scu = d->echo_scu_creators[type]();

    return scu;
}

medAbstractPacsFindScu *medAbstractPacsFactory::createFindScu(QString type)
{
    if(!d->find_scu_creators.contains(type))
        return NULL;

    medAbstractPacsFindScu *scu = d->find_scu_creators[type]();

    return scu;
}

medAbstractPacsMoveScu *medAbstractPacsFactory::createMoveScu( QString type )
{
    if(!d->move_scu_creators.contains(type))
        return NULL;

    medAbstractPacsMoveScu *scu = d->move_scu_creators[type]();

    return scu;
}

medAbstractPacsStoreScu * medAbstractPacsFactory::createStoreScu( QString type )
{
    if(!d->store_scu_creators.contains(type))
        return NULL;

    medAbstractPacsStoreScu *scu = d->store_scu_creators[type]();

    return scu;
}

medAbstractPacsStoreScp *medAbstractPacsFactory::createStoreScp(QString type)
{
    if(!d->store_scp_creators.contains(type))
        return NULL;

    medAbstractPacsStoreScp *scp = d->store_scp_creators[type]();

    return scp;
}

bool medAbstractPacsFactory::registerEchoScuType( QString type, medAbstractPacsEchoScuCreator func )
{
    if(!d->echo_scu_creators.contains(type)) {
        d->echo_scu_creators.insert(type, func);
        return true;
    }

    return false;
}

bool medAbstractPacsFactory::registerFindScuType(QString type, medAbstractPacsFindScuCreator func)
{
    if(!d->find_scu_creators.contains(type)) {
        d->find_scu_creators.insert(type, func);
        return true;
    }
 
    return false;
}

bool medAbstractPacsFactory::registerMoveScuType( QString type, medAbstractPacsMoveScuCreator func )
{
    if(!d->move_scu_creators.contains(type)) {
        d->move_scu_creators.insert(type, func);
        return true;
    }

    return false;
}

bool medAbstractPacsFactory::registerStoreScpType( QString type, medAbstractPacsStoreScpCreator func )
{
    if(!d->store_scp_creators.contains(type)) {
        d->store_scp_creators.insert(type, func);
        return true;
    }

    return false;
}

bool medAbstractPacsFactory::registerStoreScuType( QString type, medAbstractPacsStoreScuCreator func )
{
    if(!d->store_scu_creators.contains(type)) {
        d->store_scu_creators.insert(type, func);
        return true;
    }

    return false;
}

medAbstractPacsFactory::medAbstractPacsFactory(void) : dtkAbstractFactory(), d(new medAbstractPacsFactoryPrivate)
{

}

medAbstractPacsFactory::~medAbstractPacsFactory(void)
{
    delete d;

    d = NULL;
}


medAbstractPacsFactory *medAbstractPacsFactory::s_instance = NULL;
