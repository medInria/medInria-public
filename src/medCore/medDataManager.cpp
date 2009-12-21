/* medDataManager.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Dec 21 08:34:55 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Dec 21 08:34:58 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 2
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

#include <QtCore>

class medDataManagerPrivate
{
public:
    QHash<medDataIndex, dtkAbstractData *> datas;
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

void medDataManager::insert(int patientId, int studyId, int seriesId, int imageId, dtkAbstractData *data)
{
    insert(medDataIndex(patientId, studyId, seriesId, imageId), data);
}

dtkAbstractData *medDataManager::data(const medDataIndex& index)
{
    return d->datas.value(index);
}

dtkAbstractData *medDataManager::data(int patientId, int studyId, int seriesId, int imageId)
{
    return data(medDataIndex(patientId, studyId, seriesId, imageId));
}

QList<dtkAbstractData *> medDataManager::dataForPatient(int patientId)
{
    QList<dtkAbstractData *> data;

    foreach(medDataIndex index, d->datas.keys())
        if(index.patientId() == patientId)
            data << d->datas.value(index);

    return data;
}

QList<dtkAbstractData *> medDataManager::dataForStudy(int studyId)
{
    QList<dtkAbstractData *> data;

    foreach(medDataIndex index, d->datas.keys())
        if(index.studyId() == studyId)
            data << d->datas.value(index);

    return data;
}

QList<dtkAbstractData *> medDataManager::dataForSeries(int seriesId)
{
    QList<dtkAbstractData *> data;

    foreach(medDataIndex index, d->datas.keys())
        if(index.seriesId() == seriesId)
            data << d->datas.value(index);

    return data;
}

QList<dtkAbstractData *> medDataManager::dataForImage(int imageId)
{
    QList<dtkAbstractData *> data;

    foreach(medDataIndex index, d->datas.keys())
        if(index.imageId() == imageId)
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
