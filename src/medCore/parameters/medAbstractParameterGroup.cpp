/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/


#include <medAbstractParameterGroup.h>

#include <QtCore>

#include <medParameterPool.h>
#include <medAbstractView.h>
#include <medAbstractLayeredView.h>
#include <medAbstractParameter.h>

class medAbstractParameterGroupPrivate
{
public:
    QString name;
    QStringList parameters;
    bool linkAll;
};

medAbstractParameterGroup::medAbstractParameterGroup(QString name, QObject *parent) : d(new medAbstractParameterGroupPrivate)
{
    setParent(parent);
    d->name = name;
    d->linkAll = false;
}

medAbstractParameterGroup::~medAbstractParameterGroup()
{

}

void medAbstractParameterGroup::setName(QString name)
{
    d->name = name;
}

QString medAbstractParameterGroup::name() const
{
    return d->name;
}

void medAbstractParameterGroup::addParameterToLink(QString parameter)
{
    if(!d->parameters.contains(parameter))
      d->parameters.append(parameter);
}

void medAbstractParameterGroup::removeParameter(QString parameter)
{
    d->parameters.removeAll(parameter);
    setLinkAllParameters(false);
}

void medAbstractParameterGroup::setParametersToLink(QStringList parameters)
{
    d->parameters = parameters;
}

void medAbstractParameterGroup::setLinkAllParameters(bool linkAll)
{
    d->linkAll = linkAll;
}

bool medAbstractParameterGroup::linkAll() const
{
    return d->linkAll;
}

QStringList medAbstractParameterGroup::parameters() const
{
    return d->parameters;
}

void medAbstractParameterGroup::saveAsPreset()
{

}


