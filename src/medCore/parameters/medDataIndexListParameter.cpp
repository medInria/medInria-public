/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDataIndexListParameter.h>
#include <medDataIndex.h>
#include <QLabel>

class medDataListParameterPrivate
{
public:
    QList <medDataIndex> values;
};

medDataIndexListParameter::medDataIndexListParameter(QString name, QObject* parent):
    medAbstractParameter(name, parent),
    d(new medDataListParameterPrivate)
{
    d->values.clear();
}

medDataIndexListParameter::~medDataIndexListParameter()
{
    delete d;
}

void medDataIndexListParameter::setValues(QList<medDataIndex> values)
{
    d->values = values;
    emit valuesChanged(d->values);
}

void medDataIndexListParameter::clear()
{
    d->values.clear();
    emit valuesChanged(d->values);
}

QList<medDataIndex> &medDataIndexListParameter::values() const
{
    return d->values;
}

QWidget* medDataIndexListParameter::getWidget()
{
    return this->getLabel();
}

void medDataListParameter::trigger()
{
    emit valuesChanged(d->values);
}
