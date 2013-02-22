/* medAbstractPacsFactory.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Nov  7 15:54:10 2008 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 14:44:31 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 60
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
    medAbstractPacsFactory::medAbstractPacsEchoScuCreatorHash echoScuCreators;
    medAbstractPacsFactory::medAbstractPacsFindScuCreatorHash findScuCreators;
    medAbstractPacsFactory::medAbstractPacsMoveScuCreatorHash moveScuCreators;
    medAbstractPacsFactory::medAbstractPacsStoreScuCreatorHash storeScuCreators;
    medAbstractPacsFactory::medAbstractPacsStoreScpCreatorHash storeScpCreators;
};

medAbstractPacsFactory *medAbstractPacsFactory::instance(void)
{
    if(!s_instance)
        s_instance = new medAbstractPacsFactory;

    return s_instance;
}

medAbstractPacsEchoScu *medAbstractPacsFactory::createEchoScu( QString type )
{
    if(!d->echoScuCreators.contains(type))
        return NULL;

    medAbstractPacsEchoScu *scu = d->echoScuCreators[type]();

    return scu;
}

medAbstractPacsFindScu *medAbstractPacsFactory::createFindScu(QString type)
{
    if(!d->findScuCreators.contains(type))
        return NULL;

    medAbstractPacsFindScu *scu = d->findScuCreators[type]();

    return scu;
}

medAbstractPacsMoveScu *medAbstractPacsFactory::createMoveScu(QString type)
{
    if(!d->moveScuCreators.contains(type))
        return NULL;

    medAbstractPacsMoveScu *scu = d->moveScuCreators[type]();

    return scu;
}

medAbstractPacsStoreScu *medAbstractPacsFactory::createStoreScu(QString type)
{
    if(!d->storeScuCreators.contains(type))
        return NULL;

    medAbstractPacsStoreScu *scu = d->storeScuCreators[type]();

    return scu;
}

medAbstractPacsStoreScp *medAbstractPacsFactory::createStoreScp(QString type)
{
    if(!d->storeScpCreators.contains(type))
        return NULL;

    medAbstractPacsStoreScp *scp = d->storeScpCreators[type]();

    return scp;
}

bool medAbstractPacsFactory::registerEchoScuType(QString type, medAbstractPacsEchoScuCreator func)
{
    if(!d->echoScuCreators.contains(type)) {
        d->echoScuCreators.insert(type, func);
        return true;
    }

    return false;
}

bool medAbstractPacsFactory::registerFindScuType(QString type, medAbstractPacsFindScuCreator func)
{
    if(!d->findScuCreators.contains(type)) {
        d->findScuCreators.insert(type, func);
        return true;
    }
 
    return false;
}

bool medAbstractPacsFactory::registerMoveScuType(QString type, medAbstractPacsMoveScuCreator func)
{
    if(!d->moveScuCreators.contains(type)) {
        d->moveScuCreators.insert(type, func);
        return true;
    }

    return false;
}

bool medAbstractPacsFactory::registerStoreScpType(QString type, medAbstractPacsStoreScpCreator func)
{
    if(!d->storeScpCreators.contains(type)) {
        d->storeScpCreators.insert(type, func);
        return true;
    }

    return false;
}

bool medAbstractPacsFactory::registerStoreScuType( QString type, medAbstractPacsStoreScuCreator func )
{
    if(!d->storeScuCreators.contains(type)) {
        d->storeScuCreators.insert(type, func);
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
