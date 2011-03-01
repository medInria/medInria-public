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
    }

    QHash<medDataIndex, dtkAbstractData *> datas;

    medAbstractDbController* getDbController()
    {
        if (dbController == NULL)
        {
         dbController = medDbControllerFactory::instance()->createDbController("dbController");
        }
        return dbController;
    }

private:
    medAbstractDbController* dbController; 
};

medDataManager *medDataManager::instance(void)
{
    if(!s_instance)
        s_instance = new medDataManager;

    return s_instance;
}

void medDataManager::insert(const medDataIndex& index, dtkAbstractData *data)
{
    d->datas.insert(index, data);
}

void medDataManager::remove(const medDataIndex& index)
{
    delete d->datas.value(index);

    d->datas.remove(index);
}

dtkAbstractData *medDataManager::data(const medDataIndex& index)
{
    // try to load the data from db
    medAbstractDbController* db = d->getDbController();

    if (!db)
    {
        qWarning() << "No dbController registered, cannot read data";
        return NULL;
    }

    dtkAbstractData* dtkdata = db->read(index);

    if (dtkdata)
    {
        return dtkdata;
    }
    else
    {
        qWarning() << "unable to open images with index:" << index.asString();
        return NULL;
    }
}

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

medDataManager::medDataManager(void) : d(new medDataManagerPrivate)
{

}

medDataManager::~medDataManager(void)
{
    delete d;

    d = NULL;
}

medDataManager *medDataManager::s_instance = NULL;
