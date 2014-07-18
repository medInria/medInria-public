/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/


#include <medViewParameterGroup.h>


#include <medParameterPool.h>
#include <medAbstractView.h>
#include <medAbstractLayeredView.h>
#include <medAbstractParameter.h>

#include <medViewContainer.h>

#include <medParameterGroupManager.h>
#include <medSettingsManager.h>


class medViewParameterGroupPrivate
{
public:
     QSet<medAbstractView*> impactedViews;
     medParameterPool *pool;
};

medViewParameterGroup::medViewParameterGroup(QString name, QObject *parent, QString workspace)
    : medAbstractParameterGroup(name, parent, workspace), d(new medViewParameterGroupPrivate)
{
    d->pool = new medParameterPool(this);

    medParameterGroupManager::instance()->registerNewGroup(this);

    connect(this, SIGNAL(groupColorChanged(QColor,QColor)), this, SLOT(updateGroupIndicators(QColor,QColor)));
}

medViewParameterGroup::~medViewParameterGroup()
{
    medParameterGroupManager::instance()->unregisterGroup(this);

    foreach(medAbstractView *view, d->impactedViews)
        removeImpactedView(view);

    d->pool->clear();

    delete d;
}

void medViewParameterGroup::addImpactedView(medAbstractView *view)
{
    if(d->impactedViews.contains(view))
        return;

    d->impactedViews.insert(view);
    connect(view, SIGNAL(closed()), this, SLOT(removeImpactedView()));

    updateParameterToLinkList(view);

    medViewContainer *container = dynamic_cast<medViewContainer*>(view->parent());
    if(container)
        container->addColorIndicator(this->color());
}

void medViewParameterGroup::removeImpactedView(medAbstractView *view)
{
    d->impactedViews.remove(view);

    updateParameterToLinkList(view);

    medViewContainer *container = dynamic_cast<medViewContainer*>(view->parent());
    if(container)
        container->removeColorIndicator(this->color());
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
    medAbstractParameterGroup::setLinkAllParameters(linkAll);

    if(linkAll)
    {
        foreach(medAbstractView *view, d->impactedViews)
        {
            updateParameterToLinkList(view);
        }
    }
}

void medViewParameterGroup::updatePool()
{
    d->pool->clear();

    foreach(medAbstractView *view, d->impactedViews)
    {
        QList<medAbstractParameter*>  params = view->linkableParameters();
        foreach(medAbstractParameter* param, params)
        {
            if(this->parametersToLink().contains(param->name()))
            {
                d->pool->append(param);
            }
        }
    }
}

void medViewParameterGroup::updateGroupIndicators(QColor oldColor, QColor newColor)
{
    foreach(medAbstractView *view, d->impactedViews)
    {
        medViewContainer *container = dynamic_cast<medViewContainer*>(view->parent());
        if(container)
        {
            container->removeColorIndicator(oldColor);
            container->addColorIndicator(newColor);
        }
    }
}

void medViewParameterGroup::updateParameterToLinkList(medAbstractView *view)
{
    if( linkAll() )
    {
        QList<medAbstractParameter*>  params = view->linkableParameters();
        foreach(medAbstractParameter* param, params)
        {
            if(!this->parametersToLink().contains(param->name()))
                this->addParameterToLink(param->name());
        }
    }
    if( !linkAll() && !this->parametersToLink().isEmpty() )
    {
        foreach(QString paramToLink, this->parametersToLink())
        {
            if(!this->parametersToLink().contains(paramToLink))
                this->addParameterToLink(paramToLink);
        }
    }
    else if( !linkAll() && !this->parametersNotToLink().isEmpty() )
    {
        QList<medAbstractParameter*>  params = view->linkableParameters();
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
