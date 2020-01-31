/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medBoolParameterGroup.h>
#include <QList>
#include <medBoolParameter.h>

class medBoolParameterGroupPrivate
{
public:
    QList <medBoolParameter *> boolParameters;
};

medBoolParameterGroup::medBoolParameterGroup(QObject *parent)
    : QObject(parent), d(new medBoolParameterGroupPrivate)
{
}

medBoolParameterGroup::~medBoolParameterGroup()
{

}

void medBoolParameterGroup::addBoolParameter(medBoolParameter *param)
{
    foreach(medBoolParameter *refParam, d->boolParameters)
    {
        if (param->match(refParam))
            return;
    }

    d->boolParameters.append(param);
    connect(param,SIGNAL(valueChanged(bool)),this,SLOT(changeActiveBoolParameter(bool)));
}

QList <medBoolParameter *> &medBoolParameterGroup::parameters()
{
    return d->boolParameters;
}

void medBoolParameterGroup::changeActiveBoolParameter(bool paramValue)
{
    if (!paramValue)
        return;

    QObject *signalSender = sender();
    foreach(medBoolParameter *param, d->boolParameters)
    {
        if (param == signalSender)
        {
            if (!param->value())
                param->setValue(true);
        }
        else
            param->setValue(false);
    }
}
