/* medPluginManager.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Oct 28 18:09:54 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Oct 28 18:52:11 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 22
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medPluginManager.h"

#include <dtkCore/dtkPluginManager.h>

class medPluginManagerPrivate
{
public:

};

medPluginManager *medPluginManager::instance(void)
{
    if(!s_instance)
        s_instance = new medPluginManager;

    return s_instance;
}

void medPluginManager::initialize(void)
{
    dtkPluginManager::instance()->initialize();
}

void medPluginManager::uninitialize(void)
{
    dtkPluginManager::instance()->uninitialize();
}

void medPluginManager::readSettings(void)
{
    dtkPluginManager::instance()->readSettings();
}

void medPluginManager::writeSettings(void)
{
    dtkPluginManager::instance()->writeSettings();
}

medPluginManager::medPluginManager(void) : QObject(), d(new medPluginManagerPrivate)
{

}

medPluginManager::~medPluginManager(void)
{
    delete d;

    d = NULL;
}

medPluginManager *medPluginManager::s_instance = NULL;
