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

#include "medAbstractView.h"

#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>

class medViewManagerPrivate
{
public:
    QHash<medDataIndex, QList<dtkSmartPointer<medAbstractView> > > views;
};

medViewManager *medViewManager::instance(void)
{
    if(!s_instance)
        s_instance = new medViewManager;

    return s_instance;
}

void medViewManager::insert(const medDataIndex& index, medAbstractView *view)
{
    d->views[index].prepend (dtkSmartPointer<medAbstractView>(view));
}

void medViewManager::remove(const medDataIndex& index, medAbstractView *view)
{
    foreach(dtkSmartPointer<medAbstractView>lview, d->views.value(index))
        if (lview==view)
            d->views[index].removeOne(view);
}

void medViewManager::remove(const medDataIndex& index)
{
    d->views.remove(index);
}

QList<medAbstractView *> medViewManager::views(const medDataIndex& index)
{
    QList<medAbstractView *> views;
    foreach( dtkSmartPointer<medAbstractView> lview, d->views.value(index) ) {
        views.push_back( lview.data() );
    }

    return views;
}

QList<medAbstractView *> medViewManager::viewsForPatient(int id)
{
    QList<medAbstractView *> views;

    foreach(medDataIndex index, d->views.keys())
        if(index.patientId() == id)
            views << this->views(index);

    return views;
}

QList<medAbstractView *> medViewManager::viewsForStudy(int id)
{
    QList<medAbstractView *> views;

    foreach(medDataIndex index, d->views.keys())
        if(index.studyId() == id)
            views <<  this->views(index);

    return views;
}

QList<medAbstractView *> medViewManager::viewsForSeries(int id)
{
    QList<medAbstractView *> views;

    foreach(medDataIndex index, d->views.keys())
        if(index.seriesId() == id)
            views <<  this->views(index);

    return views;
}

QList<medAbstractView *> medViewManager::viewsForImage(int id)
{
    QList<medAbstractView *> views;

    foreach(medDataIndex index, d->views.keys())
        if(index.imageId() == id)
            views <<  this->views(index);

    return views;
}

//! Get the index associated to a view.
/*! 
 *  Warning, this method is quite consuming, use this as little as
 *  possible.
 */

medDataIndex medViewManager::index(medAbstractView *view)
{
    foreach(medDataIndex index, d->views.keys())
        foreach(medAbstractView *v, d->views.value(index))
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
