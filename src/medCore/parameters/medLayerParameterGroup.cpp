/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medLayerParameterGroup.h"

#include <medParameterPool.h>
#include <medAbstractView.h>
#include <medAbstractLayeredView.h>
#include <medAbstractParameter.h>

#include <medParameterGroupManager.h>
#include <medSettingsManager.h>

class medLayerParameterGroupPrivate
{
public:
    QMultiHash<medAbstractLayeredView*, medAbstractData*> impactedLayers;
    medParameterPool *pool;
};


medLayerParameterGroup::medLayerParameterGroup(QString name, QObject *parent, QString workspace)
    : medAbstractParameterGroup(name, parent, workspace), d(new medLayerParameterGroupPrivate)
{
    d->pool = new medParameterPool(this);

    medParameterGroupManager::instance()->registerNewGroup(this);
}

medLayerParameterGroup::~medLayerParameterGroup()
{
    medParameterGroupManager::instance()->unregisterGroup(this);

    QHashIterator<medAbstractLayeredView*, medAbstractData*> iter(d->impactedLayers);
    while(iter.hasNext())
    {
        iter.next();
        removeImpactedlayer(iter.key(), iter.value());
    }

    d->pool->clear();

    delete d;
}

void medLayerParameterGroup::addImpactedlayer(medAbstractLayeredView *view, medAbstractData* layerData)
{    
    d->impactedLayers.insert(view, layerData);

    updateParameterToLinkList(view, layerData);

    connect(view, SIGNAL(layerRemoved(medAbstractData*)), this, SLOT(removeImpactedlayer(medAbstractData*)));
}

void medLayerParameterGroup::removeImpactedlayer(medAbstractLayeredView *view,  medAbstractData* layerData)
{
    d->impactedLayers.remove(view, layerData);

    updateParameterToLinkList(view, layerData);
}

void medLayerParameterGroup::removeImpactedlayer(medAbstractData* layerData)
{
    medAbstractLayeredView *view = dynamic_cast<medAbstractLayeredView *>(this->sender());
    removeImpactedlayer(view, layerData);
}

void medLayerParameterGroup::setLinkAllParameters(bool linkAll)
{
    medAbstractParameterGroup::setLinkAllParameters(linkAll);

    if( linkAll )
    {
        QHashIterator<medAbstractLayeredView*, medAbstractData*> iter(d->impactedLayers);
        while(iter.hasNext())
        {
            iter.next();

            medAbstractLayeredView* view = iter.key();
            medAbstractData* layerData = iter.value();

            updateParameterToLinkList(view, layerData);
        }
    }
}

QMultiHash<medAbstractLayeredView*, medAbstractData*> medLayerParameterGroup::impactedLayers()
{
    return d->impactedLayers;
}

void medLayerParameterGroup::updatePool()
{
    d->pool->clear();

    QHashIterator<medAbstractLayeredView*, medAbstractData*> iter(d->impactedLayers);
    while(iter.hasNext())
    {
        iter.next();
        QList<medAbstractParameter*>  params;

        medAbstractLayeredView* view = iter.key();
        medAbstractData* layerData = iter.value();

        if(!view->contains(layerData))
            continue;

        foreach(medAbstractInteractor* interactor, view->layerInteractors(view->layer(layerData)))
            params.append(interactor->linkableParameters());

        foreach(medAbstractParameter* param, params)
        {
            if(this->parametersToLink().contains(param->name()))
                d->pool->append(param);
        }
    }
}


void medLayerParameterGroup::updateParameterToLinkList(medAbstractLayeredView *view, medAbstractData* layerData)
{
    if( linkAll() && view->contains(layerData) )
    {
        QList<medAbstractParameter*>  params;

        foreach(medAbstractInteractor* interactor, view->layerInteractors(view->layer(layerData)))
            params.append(interactor->linkableParameters());

        foreach(medAbstractParameter* param, params)
        {
            if(!this->parametersToLink().contains(param->name()))
                this->addParameterToLink(param->name());
        }
    }
    else if( !linkAll() && !this->parametersNotToLink().isEmpty() && view->contains(layerData) )
    {
        QList<medAbstractParameter*>  params;

        foreach(medAbstractInteractor* interactor, view->layerInteractors(view->layer(layerData)))
            params.append(interactor->linkableParameters());

        foreach(medAbstractParameter* param, params)
        {
            if(!this->parametersToLink().contains(param->name()))
                this->addParameterToLink(param->name());
        }

        foreach(QString paramNotToLink, this->parametersNotToLink())
        {
            if(this->parametersToLink().contains(paramNotToLink))
                this->removeParameter(paramNotToLink);
        }
    }

    updatePool();
}
