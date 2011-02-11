/* medViewManager.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Dec 21 12:47:51 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Dec 20 08:52:49 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 38
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
    d->views[index].prepend (view);
}

void medViewManager::remove(const medDataIndex& index, dtkAbstractView *view)
{
    foreach(dtkAbstractView *lview, d->views.value(index))
        if (lview==view)
            d->views[index].removeOne(view);
    
    dtkAbstractViewFactory::instance()->destroy (view);
}

void medViewManager::remove(const medDataIndex& index)
{
     foreach(dtkAbstractView *view, d->views.value(index))
         dtkAbstractViewFactory::instance()->destroy (view);

    d->views.remove(index);
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

//! Get the index associated to a view.
/*! 
 *  Warning, this method is quite consuming, use this as little as
 *  possible.
 */

medDataIndex medViewManager::index(dtkAbstractView *view)
{
    foreach(medDataIndex index, d->views.keys())
        foreach(dtkAbstractView *v, d->views.value(index))
            if(v == view)
                return index;

    return medDataIndex();
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
