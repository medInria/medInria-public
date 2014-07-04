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
    QMultiHash<medAbstractLayeredView*, unsigned int> impactedLayers;
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

    QHashIterator<medAbstractLayeredView*, unsigned int> iter(d->impactedLayers);
    while(iter.hasNext())
    {
        iter.next();
        removeImpactedlayer(iter.key(), iter.value());
    }

    d->pool->clear();
}

void medLayerParameterGroup::addImpactedlayer(medAbstractLayeredView *view, unsigned int layer)
{    
    d->impactedLayers.insert(view, layer);

    connect(view, SIGNAL(layerRemoved(uint)), this, SLOT(removeImpactedlayer(uint)));

    if(linkAll())
    {
        QList<medAbstractParameter*>  params;

        foreach(medAbstractInteractor* interactor, view->interactors(layer))
            params.append(interactor->linkableParameters());

        foreach(medAbstractParameter* param, params)
        {
            if(!this->parameters().contains(param->name()))
                this->addParameterToLink(param->name());
        }
    }
}

void medLayerParameterGroup::removeImpactedlayer(medAbstractLayeredView *view, unsigned int layer)
{
    d->impactedLayers.remove(view, layer);
}

void medLayerParameterGroup::removeImpactedlayer(uint layer)
{
    medAbstractLayeredView *view = dynamic_cast<medAbstractLayeredView *>(this->sender());
    removeImpactedlayer(view, layer);
}

void medLayerParameterGroup::setLinkAllParameters(bool linkAll)
{
    medAbstractParameterGroup::setLinkAllParameters(linkAll);

    if( linkAll )
    {
        QHashIterator<medAbstractLayeredView*, unsigned int> iter(d->impactedLayers);
        while(iter.hasNext())
        {
            iter.next();
            QList<medAbstractParameter*>  params;

            medAbstractLayeredView* view = iter.key();
            unsigned int layer = iter.value();

            foreach(medAbstractInteractor* interactor, view->interactors(layer))
                params.append(interactor->linkableParameters());

            foreach(medAbstractParameter* param, params)
            {
                if(!this->parameters().contains(param->name()))
                    this->addParameterToLink(param->name());
            }
        }
    }
}

QMultiHash<medAbstractLayeredView*, unsigned int> medLayerParameterGroup::impactedLayers()
{
    return d->impactedLayers;
}

void medLayerParameterGroup::update()
{
    d->pool->clear();

    QHashIterator<medAbstractLayeredView*, unsigned int> iter(d->impactedLayers);
    while(iter.hasNext())
    {
        iter.next();
        QList<medAbstractParameter*>  params;

        medAbstractLayeredView* view = iter.key();
        unsigned int layer = iter.value();

        foreach(medAbstractInteractor* interactor, view->interactors(layer))
            params.append(interactor->linkableParameters());

        foreach(medAbstractParameter* param, params)
        {
            if(this->parameters().contains(param->name()))
                d->pool->append(param);
        }
    }
}
