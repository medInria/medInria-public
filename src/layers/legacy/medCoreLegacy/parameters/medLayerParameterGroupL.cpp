/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medLayerParameterGroupL.h"

#include <medParameterPoolL.h>
#include <medAbstractView.h>
#include <medAbstractLayeredView.h>
#include <medAbstractParameterL.h>

#include <medParameterGroupManagerL.h>
#include <medSettingsManager.h>

class medLayerParameterGroupLL
{
public:
    QMultiHash<medAbstractLayeredView*, medAbstractData*> impactedLayers;
    medParameterPoolL *pool;
};


medLayerParameterGroupL::medLayerParameterGroupL(QString name, QObject *parent, QString workspace)
    : medAbstractParameterGroupL(name, parent, workspace), d(new medLayerParameterGroupLL)
{
    d->pool = new medParameterPoolL(this);

    medParameterGroupManagerL::instance().registerNewGroup(this);
}

medLayerParameterGroupL::~medLayerParameterGroupL()
{
    medParameterGroupManagerL::instance().unregisterGroup(this);

    QHashIterator<medAbstractLayeredView*, medAbstractData*> iter(d->impactedLayers);
    while(iter.hasNext())
    {
        iter.next();
        removeImpactedlayer(iter.key(), iter.value());
    }

    d->pool->clear();

    delete d;
}

void medLayerParameterGroupL::addImpactedlayer(medAbstractLayeredView *view, medAbstractData* layerData)
{
    d->impactedLayers.insert(view, layerData);

    updateParameterToLinkList(view, layerData);

    connect(view, SIGNAL(layerRemoved(medAbstractData*)), this, SLOT(removeImpactedlayer(medAbstractData*)));
}

void medLayerParameterGroupL::removeImpactedlayer(medAbstractLayeredView *view,  medAbstractData* layerData)
{
    d->impactedLayers.remove(view, layerData);

    updateParameterToLinkList(view, layerData);
}

void medLayerParameterGroupL::removeImpactedlayer(medAbstractData* layerData)
{
    medAbstractLayeredView *view = dynamic_cast<medAbstractLayeredView *>(this->sender());
    removeImpactedlayer(view, layerData);
}

void medLayerParameterGroupL::setLinkAllParameters(bool linkAll)
{
    medAbstractParameterGroupL::setLinkAllParameters(linkAll);

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

QMultiHash<medAbstractLayeredView*, medAbstractData*> medLayerParameterGroupL::impactedLayers()
{
    return d->impactedLayers;
}

void medLayerParameterGroupL::updatePool()
{
    d->pool->clear();

    QHashIterator<medAbstractLayeredView*, medAbstractData*> iter(d->impactedLayers);
    while(iter.hasNext())
    {
        iter.next();
        QList<medAbstractParameterL*>  params;

        medAbstractLayeredView* view = iter.key();
        medAbstractData* layerData = iter.value();

        if(!view->contains(layerData))
            continue;

        for(medAbstractInteractor* interactor : view->layerInteractors(view->layer(layerData)))
        {
            params.append(interactor->linkableParameters());
        }

        for(medAbstractParameterL* param : params)
        {
            if(this->parametersToLink().contains(param->name()))
            {
                d->pool->append(param);
            }
        }
    }
}


void medLayerParameterGroupL::updateParameterToLinkList(medAbstractLayeredView *view, medAbstractData* layerData)
{
    if( linkAll() && view->contains(layerData) )
    {
        QList<medAbstractParameterL*>  params;

        for(medAbstractInteractor* interactor : view->layerInteractors(view->layer(layerData)))
        {
            params.append(interactor->linkableParameters());
        }

        for(medAbstractParameterL* param : params)
        {
            if(!this->parametersToLink().contains(param->name()))
            {
                this->addParameterToLink(param->name());
            }
        }
    }
    else if( !linkAll() && !this->parametersNotToLink().isEmpty() && view->contains(layerData) )
    {
        QList<medAbstractParameterL*>  params;

        for(medAbstractInteractor* interactor : view->layerInteractors(view->layer(layerData)))
        {
            params.append(interactor->linkableParameters());
        }

        for(medAbstractParameterL* param : params)
        {
            if(!this->parametersToLink().contains(param->name()))
            {
                this->addParameterToLink(param->name());
            }
        }

        for(QString paramNotToLink : this->parametersNotToLink())
        {
            if(this->parametersToLink().contains(paramNotToLink))
            {
                this->removeParameter(paramNotToLink);
            }
        }
    }

    updatePool();
}
