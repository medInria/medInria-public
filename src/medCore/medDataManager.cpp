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

    QHash<medDataIndex, dtkAbstractData *> datas;
    QHash<medDataIndex, dtkAbstractData *> volatileDatas;

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

/*
void medDataManager::insert(const medDataIndex& index, dtkAbstractData *data)
{
    d->datas.insert(index, data);
}

void medDataManager::remove(const medDataIndex& index)
{
    delete d->datas.value(index);

    d->datas.remove(index);
}
*/

dtkAbstractData *medDataManager::data(const medDataIndex& index)
{
    dtkAbstractData* dtkdata = NULL;

    bool newData = false;

    // try to get it from cache first
    if ( d->datas.contains(index) || d->volatileDatas.contains (index) )
    {
        qDebug() << "Reading from cache";
	if (d->datas.contains(index))
	  dtkdata = d->datas.value(index);
	else
	  dtkdata = d->volatileDatas.value(index);
    }
    else
    {
        qDebug() << "Reading from db";

        // try to load the data from db
        medAbstractDbController* db = d->getDbController();
        if (db)
        {
            dtkdata = db->read(index);
	    if (dtkdata) {
	        d->datas[index] = dtkdata;
		newData = true;
	    }
        }

        //if the data is still invalid we continue in the non-pers db
        if (!dtkdata)
        {
            medAbstractDbController* npDb = d->getNonPersDbController();
            if(npDb)
            {
                dtkdata = npDb->read(index);
		if (dtkdata) {
		    d->volatileDatas[index] = dtkdata;
		    newData = true;
		}
            }
        }
    }

    if (dtkdata)
    {
        if (newData)
	    emit dataAdded (index.patientId());
        return dtkdata;
    }
    else
    {
        qWarning() << "unable to open images with index:" << index.asString();
        return NULL;
    }
}

medDataIndex medDataManager::import (dtkAbstractData *data)
{
    if (!data)
        return medDataIndex();

    bool newData = false;
    
    medDataIndex index;
    
    medAbstractDbController* db = d->getDbController();
    if(db)
    {
        index = db->import(data);
    }

    if (!index.isValid()) {
        qWarning() << "index is not valid";
        return index;
    }

    d->datas[index] = data;

    emit dataAdded (index.patientId());
    
    return index;
}

medDataIndex medDataManager::importNonPersistent (dtkAbstractData *data)
{
    if (!data)
        return medDataIndex();

    foreach (dtkAbstractData *dtkdata, d->datas) {
        if (data==dtkdata) {
	    qWarning() << "data already in manager, skipping";
	    return medDataIndex();
	}
    }

    foreach (dtkAbstractData *dtkdata, d->volatileDatas) {
        if (data==dtkdata) {
	    qWarning() << "data already in manager, skipping";
	    return medDataIndex();
	}
    }

    medDataIndex index;
    
    medAbstractDbController* npDb = d->getNonPersDbController();
    if(npDb)
    {
        index = npDb->import(data);
    }

    if (!index.isValid()) {
        qWarning() << "index is not valid";
        return index;
    }
    
    if (d->volatileDatas.contains (index)) {
        qWarning() << "index already in manager, skipping";
	return index;
    }

    d->volatileDatas[index] = data;

    emit dataAdded (index.patientId());
    
    return index;
}

void medDataManager::storeNonPersistentDataToDatabase (void)
{
    foreach (dtkAbstractData *dtkdata, d->volatileDatas) {
        this->import (dtkdata);
    }
    
    if (medAbstractDbController* npDb = d->getNonPersDbController())
        npDb->clear();

    d->volatileDatas.clear();
}


int medDataManager::nonPersistentDataCount (void) const
{
    return d->volatileDatas.count();
}

void medDataManager::clearNonPersistentData (void)
{
    if (medAbstractDbController* npDb = d->getNonPersDbController())
        npDb->clear();

    d->volatileDatas.clear();
}

/*
QList<dtkAbstractData *> medDataManager::dataForPatient(int id)
{
    QList<dtkAbstractData *> data;

    foreach(medDataIndex index, d->datas.keys())
        if(index.patientId() == id)
            data << d->datas.value(index);

    return data;
}

QList<dtkAbstractData *> medDataManager::dataForStudy(int id)
{
    QList<dtkAbstractData *> data;

    foreach(medDataIndex index, d->datas.keys())
        if(index.studyId() == id)
            data << d->datas.value(index);

    return data;
}

QList<dtkAbstractData *> medDataManager::dataForSeries(int id)
{
    QList<dtkAbstractData *> data;

    foreach(medDataIndex index, d->datas.keys())
        if(index.seriesId() == id)
            data << d->datas.value(index);

    return data;
}

QList<dtkAbstractData *> medDataManager::dataForImage(int id)
{
    QList<dtkAbstractData *> data;

    foreach(medDataIndex index, d->datas.keys())
        if(index.imageId() == id)
            data << d->datas.value(index);

    return data;
}
*/
medDataManager::medDataManager(void) : d(new medDataManagerPrivate)
{

}

medDataManager::~medDataManager(void)
{
    delete d;

    d = NULL;
}

medDataManager *medDataManager::s_instance = NULL;
