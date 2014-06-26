/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/


#include <medParameterGroup.h>

#include <QtCore>

#include <medParameterPool.h>
#include <medAbstractView.h>
#include <medAbstractLayeredView.h>
#include <medAbstractParameter.h>

class medParameterGroupPrivate
{
public:
    QString name;
    QStringList parameters;

    QSet<medAbstractView*> impactedViews;
    QMultiHash<medAbstractLayeredView*, unsigned int> impactedLayers;
    medParameterPool *pool;
};

medParameterGroup::medParameterGroup(QString name, QObject *parent) : d(new medParameterGroupPrivate)
{
    setParent(parent);
    d->pool = new medParameterPool(this);
    d->name = name;
}

medParameterGroup::~medParameterGroup()
{

}

void medParameterGroup::setName(QString name)
{
    d->name = name;
}

QString medParameterGroup::name() const
{
    return d->name;
}

void medParameterGroup::addParameter(QString parameter)
{
    d->parameters.append(parameter);
}

void medParameterGroup::removeParameter(QString parameter)
{
    d->parameters.removeAll(parameter);
}

void medParameterGroup::setParameters(QStringList parameters)
{
    d->parameters = parameters;
}

QStringList medParameterGroup::parameters() const
{
    return d->parameters;
}

void medParameterGroup::addImpactedView(medAbstractView *view)
{
    d->impactedViews.insert(view);
}

void medParameterGroup::removeImpactedView(medAbstractView *view)
{
    d->impactedViews.remove(view);
}

QList<medAbstractView*> medParameterGroup::impactedViews()
{
    return d->impactedViews.toList();
}

void medParameterGroup::addImpactedlayer(medAbstractLayeredView *view, unsigned int layer)
{
    d->impactedLayers.insert(view, layer);
}

void medParameterGroup::removeImpactedlayer(medAbstractLayeredView *view, unsigned int layer)
{
    d->impactedLayers.remove(view, layer);
}

QMultiHash<medAbstractLayeredView*, unsigned int> medParameterGroup::impactedLayers()
{
    return d->impactedLayers;
}

void medParameterGroup::saveAsPreset()
{

}

void medParameterGroup::update()
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
            if(d->parameters.contains(param->name()))
            {
                d->pool->append(param);
            }
        }
    }
}
