/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/


#include "medViewParameterGroup.h"


#include <medParameterPool.h>
#include <medAbstractView.h>
#include <medAbstractLayeredView.h>
#include <medAbstractParameter.h>


class medViewParameterGroupPrivate
{
public:
     QSet<medAbstractView*> impactedViews;
     bool linkAll;
     medParameterPool *pool;
};

medViewParameterGroup::medViewParameterGroup(QString name, QObject *parent) : medAbstractParameterGroup(name, parent),
    d(new medViewParameterGroupPrivate)
{
    d->linkAll = false;
    d->pool = new medParameterPool(this);
}

medViewParameterGroup::~medViewParameterGroup()
{
    d->pool->clear();
}

void medViewParameterGroup::addImpactedView(medAbstractView *view)
{
    d->impactedViews.insert(view);
    connect(view, SIGNAL(closed()), this, SLOT(removeImpactedView()));

    if(linkAll())
    {
        QList<medAbstractParameter*>  params;

        params.append(view->primaryNavigator()->linkableParameters());
        foreach(medAbstractNavigator* nav,  view->extraNavigators())
            params.append(nav->linkableParameters());

        foreach(medAbstractParameter* param, params)
        {
            if(!this->parameters().contains(param->name()))
                this->addParameterToLink(param->name());
        }
    }
}

void medViewParameterGroup::removeImpactedView(medAbstractView *view)
{
    d->impactedViews.remove(view);
}

void medViewParameterGroup::removeImpactedView()
{
    medAbstractView* view = dynamic_cast<medAbstractView*>(this->sender());
    removeImpactedView(view);
}

QList<medAbstractView*> medViewParameterGroup::impactedViews()
{
    return d->impactedViews.toList();
}

void medViewParameterGroup::setLinkAllParameters(bool linkAll)
{
    d->linkAll = linkAll;

    foreach(medAbstractView *view, d->impactedViews)
    {
        QList<medAbstractParameter*>  params;

        params.append(view->primaryNavigator()->linkableParameters());
        foreach(medAbstractNavigator* nav,  view->extraNavigators())
            params.append(nav->linkableParameters());

        foreach(medAbstractParameter* param, params)
        {
            if(!this->parameters().contains(param->name()))
                this->addParameterToLink(param->name());
        }
    }

}

bool medViewParameterGroup::linkAll() const
{
    return d->linkAll;
}

void medViewParameterGroup::update()
{
    d->pool->clear();

    foreach(medAbstractView *view, d->impactedViews)
    {
        QList<medAbstractParameter*>  params;

        params.append(view->primaryNavigator()->linkableParameters());
        foreach(medAbstractNavigator* nav,  view->extraNavigators())
            params.append(nav->linkableParameters());

        foreach(medAbstractParameter* param, params)
        {
            if(this->parameters().contains(param->name()))
            {
                d->pool->append(param);
            }
        }
    }
}
