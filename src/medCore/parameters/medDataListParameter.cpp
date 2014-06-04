/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDataListParameter.h>
#include <medAbstractData.h>

#include <QDebug>

class medDataListParameterPrivate
{
public:
    QList <medAbstractData *> values;
};

medDataListParameter::medDataListParameter(QString name, QObject* parent):
    medAbstractParameter(name, parent),
    d(new medDataListParameterPrivate)
{
    d->values.clear();
}

medDataListParameter::~medDataListParameter()
{
    delete d;
}

void medDataListParameter::setValues(QList <medAbstractData *> values)
{
    d->values = values;
    emit valuesChanged(d->values);
}

void medDataListParameter::clear()
{
    d->values.clear();
    emit valuesChanged(d->values);
}

QList <medAbstractData *> medDataListParameter::values() const
{
    return d->values;
}

QWidget* medDataListParameter::getWidget()
{
    return this->getLabel();
}
