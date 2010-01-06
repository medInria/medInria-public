/* medViewManager.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Dec 21 12:47:51 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Dec 21 12:47:51 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medViewManager.h"

class medViewManagerPrivate
{
public:
};

medViewManager *medViewManager::instance(void)
{
    if(!s_instance)
        s_instance = new medViewManager;

    return s_instance;
}

medViewManager::medViewManager(void) : d(new medViewManagerPrivate)
{

}

medViewManager::~medViewManager(void)
{
    delete d;

    d = NULL;
}

medViewManager *medViewManager::s_instance = NULL;
