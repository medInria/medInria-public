/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/


#include <medViewParameterGroupL.h>


#include <medParameterPoolL.h>
#include <medAbstractView.h>
#include <medAbstractLayeredView.h>
#include <medAbstractParameterL.h>

#include <medViewContainer.h>

#include <medParameterGroupManagerL.h>
#include <medSettingsManager.h>


class medViewParameterGroupLPrivate
{
public:
     QSet<medAbstractView*> impactedViews;
     medParameterPoolL *pool;
};

medViewParameterGroupL::medViewParameterGroupL(QString name, QObject *parent, QString workspace)
    : medAbstractParameterGroupL(name, parent, workspace), d(new medViewParameterGroupLPrivate)
{
    d->pool = new medParameterPoolL(this);

    medParameterGroupManagerL::instance().registerNewGroup(this);

    connect(this, SIGNAL(groupColorChanged(QColor,QColor)), this, SLOT(updateGroupIndicators(QColor,QColor)));
}

medViewParameterGroupL::~medViewParameterGroupL()
{
    medParameterGroupManagerL::instance().unregisterGroup(this);

    d->pool->clear();

    delete d;
}

void medViewParameterGroupL::addImpactedView(medAbstractView *view)
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

void medViewParameterGroupL::removeImpactedView(medAbstractView *view)
{
    d->impactedViews.remove(view);

    updateParameterToLinkList(view);

    medViewContainer *container = dynamic_cast<medViewContainer*>(view->parent());
    if(container)
        container->removeColorIndicator(this->color());
}

void medViewParameterGroupL::removeImpactedView()
{
    medAbstractView* view = dynamic_cast<medAbstractView*>(this->sender());
    removeImpactedView(view);
}

QList<medAbstractView*> medViewParameterGroupL::impactedViews()
{
    return d->impactedViews.toList();
}

void medViewParameterGroupL::setLinkAllParameters(bool linkAll)
{
    medAbstractParameterGroupL::setLinkAllParameters(linkAll);

    if(linkAll)
    {
        for(medAbstractView *view : d->impactedViews)
        {
            updateParameterToLinkList(view);
        }
    }
}

void medViewParameterGroupL::updatePool()
{
    d->pool->clear();

    for(medAbstractView *view : d->impactedViews)
    {
        QList<medAbstractParameterL*>  params = view->linkableParameters();
        for(medAbstractParameterL* param : params)
        {
            if(this->parametersToLink().contains(param->name()))
            {
                d->pool->append(param);
            }
        }
    }
}

void medViewParameterGroupL::updateGroupIndicators(QColor oldColor, QColor newColor)
{
    for(medAbstractView *view : d->impactedViews)
    {
        medViewContainer *container = dynamic_cast<medViewContainer*>(view->parent());
        if(container)
        {
            container->removeColorIndicator(oldColor);
            container->addColorIndicator(newColor);
        }
    }
}

void medViewParameterGroupL::updateParameterToLinkList(medAbstractView *view)
{
    if( linkAll() )
    {
        QList<medAbstractParameterL*>  params = view->linkableParameters();
        for(medAbstractParameterL* param : params)
        {
            if(!this->parametersToLink().contains(param->name()))
            {
                this->addParameterToLink(param->name());
            }
        }
    }
    if( !linkAll() && !this->parametersToLink().isEmpty() )
    {
        for(QString paramToLink : this->parametersToLink())
        {
            if(!this->parametersToLink().contains(paramToLink))
            {
                this->addParameterToLink(paramToLink);
            }
        }
    }
    else if( !linkAll() && !this->parametersNotToLink().isEmpty() )
    {
        QList<medAbstractParameterL*>  params = view->linkableParameters();
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
