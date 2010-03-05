/* medViewManager.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Dec 21 12:47:51 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Fri Mar  5 09:06:06 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 18
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medViewManager.h"

#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>

class medViewManagerPrivate
{
public:
    QHash<medDataIndex, QList<dtkAbstractView *> > views;
};

medViewManager *medViewManager::instance(void)
{
    if(!s_instance)
        s_instance = new medViewManager;

    return s_instance;
}

void medViewManager::insert(const medDataIndex& index, dtkAbstractView *view)
{
    d->views[index] << view;

    qDebug() << __func__ << d->views;
}

QList<dtkAbstractView *> medViewManager::views(const medDataIndex& index)
{
    return d->views.value(index);
}

QList<dtkAbstractView *> medViewManager::viewsForPatient(int id)
{
    QList<dtkAbstractView *> views;

    foreach(medDataIndex index, d->views.keys())
        if(index.patientId() == id)
            views << d->views.value(index);

    return views;
}

QList<dtkAbstractView *> medViewManager::viewsForStudy(int id)
{
    QList<dtkAbstractView *> views;

    foreach(medDataIndex index, d->views.keys())
        if(index.studyId() == id)
            views << d->views.value(index);

    return views;
}

QList<dtkAbstractView *> medViewManager::viewsForSeries(int id)
{
    QList<dtkAbstractView *> views;

    foreach(medDataIndex index, d->views.keys())
        if(index.seriesId() == id)
            views << d->views.value(index);

    return views;
}

QList<dtkAbstractView *> medViewManager::viewsForImage(int id)
{
    QList<dtkAbstractView *> views;

    foreach(medDataIndex index, d->views.keys())
        if(index.imageId() == id)
            views << d->views.value(index);

    return views;
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
