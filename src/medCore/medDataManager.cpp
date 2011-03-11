/* medDataManager.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Dec 21 08:34:55 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Mar 17 18:48:48 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 17
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medDataManager.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include "medDbControllerFactory.h"
#include "medAbstractDbController.h"

#include <QtCore>

class medDataManagerPrivate
{
public:

    medDataManagerPrivate()
    {
        dbController = NULL;
        nonPersDbController = NULL;
    }
    // this is the view cache
    QHash<medDataIndex, QSharedPointer<dtkAbstractData>> dataCache;

    medAbstractDbController* getDbController()
    {
        if (dbController == NULL)
        {
         dbController = medDbControllerFactory::instance()->createDbController("DbController");
         if (!dbController)
             qWarning() << "No dbController registered!";
        }
        return dbController;
    }

    medAbstractDbController* getNonPersDbController()
    {
        if (nonPersDbController == NULL)
        {
            nonPersDbController  = medDbControllerFactory::instance()->createDbController("NonPersistentDbController");
            if (!nonPersDbController)
                qWarning() << "No nonPersistentDbController registered!";
        }
        return nonPersDbController ;
    }

private:
    medAbstractDbController* dbController; 
    medAbstractDbController* nonPersDbController; 

};

medDataManager *medDataManager::instance(void)
{
    if(!s_instance)
        s_instance = new medDataManager;

    return s_instance;
}


QSharedPointer<dtkAbstractData> medDataManager::data(const medDataIndex& index)
{
    QSharedPointer<dtkAbstractData>  dtkdata;

    // try to get it from cache first
    if ( d->dataCache.contains(index) )
    {
        qDebug() << "Reading from cache";
        dtkdata = d->dataCache.value(index);
    }
    else
    {
        qDebug() << "Reading from db";

        // try to load the data from db
        medAbstractDbController* db = d->getDbController();
        if (db)
        {
            dtkdata = db->read(index);
        }

        //if the data is still invalid we continue in the non-pers db
        if (!dtkdata)
        {
            medAbstractDbController* npDb = d->getNonPersDbController();
            if(npDb)
            {
                dtkdata = npDb->read(index);
            }
        }

        // store it in the cache
        if (dtkdata)
        {
            d->dataCache[index] = dtkdata;
        }
    }

    if (!dtkdata)
    {
        qWarning() << "unable to open images with index:" << index.asString();
    }
    return dtkdata;

}

medDataManager::medDataManager(void) : d(new medDataManagerPrivate)
{

}

medDataManager::~medDataManager(void)
{
    delete d;

    d = NULL;
}

void medDataManager::destroy( void )
{

}


medDataManager *medDataManager::s_instance = NULL;
