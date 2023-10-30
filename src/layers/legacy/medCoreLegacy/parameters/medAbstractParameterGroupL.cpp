/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/


#include <medAbstractParameterGroupL.h>

#include <QtCore>

#include <medAbstractView.h>
#include <medAbstractLayeredView.h>
#include <medAbstractParameterL.h>
#include <medParameterGroupManagerL.h>

class medAbstractParameterGroupLPrivate
{
public:
    QString name;
    QStringList paramsLinked;
    QStringList paramsNotLinked;
    bool linkAll;
    QColor color;
    QString workspace;
};

medAbstractParameterGroupL::medAbstractParameterGroupL(QString name, QObject *parent, QString workspace) : d(new medAbstractParameterGroupLPrivate)
{
    setParent(parent);
    d->name = name;
    d->linkAll = false;
    d->color = QColor::fromHsv(qrand()%360, 255, 210);
    d->workspace = workspace;
}

medAbstractParameterGroupL::~medAbstractParameterGroupL()
{
    medParameterGroupManagerL::instance().unregisterGroup(this);

    delete d;
}

void medAbstractParameterGroupL::setName(QString name)
{
    d->name = name;
}

QString medAbstractParameterGroupL::name() const
{
    return d->name;
}

void medAbstractParameterGroupL::addParameterToLink(QString parameter)
{
    if(!d->paramsLinked.contains(parameter))
      d->paramsLinked.append(parameter);

    updatePool();
}

void medAbstractParameterGroupL::removeParameter(QString parameter)
{
    d->paramsLinked.removeAll(parameter);

    if(!d->paramsNotLinked.contains(parameter))
      d->paramsNotLinked.append(parameter);

    setLinkAllParameters(false);

    updatePool();
}

void medAbstractParameterGroupL::setParametersToLink(QStringList parameters)
{
    d->paramsLinked = parameters;
}

void medAbstractParameterGroupL::setLinkAllParameters(bool linkAll)
{
    d->linkAll = linkAll;
}

bool medAbstractParameterGroupL::linkAll() const
{
    return d->linkAll;
}

QStringList medAbstractParameterGroupL::parametersToLink() const
{
    return d->paramsLinked;
}

QStringList medAbstractParameterGroupL::parametersNotToLink() const
{
    return d->paramsNotLinked;
}

void medAbstractParameterGroupL::setColor(QColor color)
{
    QColor oldColor = d->color;
    d->color = color;
    emit groupColorChanged(oldColor, color);
}

QColor medAbstractParameterGroupL::color() const
{
    return d->color;
}

void medAbstractParameterGroupL::setWorkspace(QString workspace)
{
    d->workspace = workspace;
}

QString medAbstractParameterGroupL::workspace() const
{
    return d->workspace;
}


