/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medViewManager.h>

#include <medAbstractView.h>

#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>


class medViewManagerPrivate
{
public:
    QHash<medDataIndex, QList<dtkSmartPointer<medAbstractView> > > views;
    QList<medAbstractView*> selectedViews;
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

void medViewManager::insert(const medDataIndex& index, medAbstractView *view)
{
    d->views[index].prepend (dtkSmartPointer<medAbstractView>(view));
    emit viewOpened();
}

void medViewManager::remove(const medDataIndex& index, medAbstractView *view)
{
    foreach(dtkSmartPointer<medAbstractView>lview, d->views.value(index))
        if (lview==view) {
            d->views[index].removeAll(lview);
        }
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

QList<medDataIndex> medViewManager::indices(medAbstractView *view) const
{
    QList<medDataIndex> indices;
    foreach(medDataIndex index, d->views.keys())
        foreach(medAbstractView *v, d->views.value(index))
            if(v == view)
                indices << index;

    return indices;
}

medViewManager *medViewManager::s_instance = NULL;
