/*=========================================================================

 medInria

 Copyright (c) INRIA 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDataListParameterL.h>
#include <medDataIndex.h>
#include <QLabel>

class medDataListParameterLPrivate
{
public:
    QList <medDataIndex> values;
};

medDataListParameterL::medDataListParameterL(QString name, QObject* parent):
    medAbstractParameterL(name, parent),
    d(new medDataListParameterLPrivate)
{
    d->values.clear();
}

medDataListParameterL::~medDataListParameterL()
{
    delete d;
}

void medDataListParameterL::setValues(QList<medDataIndex> values)
{
    d->values = values;
    emit valuesChanged(d->values);
}

void medDataListParameterL::clear()
{
    d->values.clear();
    emit valuesChanged(d->values);
}

QList<medDataIndex> &medDataListParameterL::values() const
{
    return d->values;
}

QWidget* medDataListParameterL::getWidget()
{
    return this->getLabel();
}

void medDataListParameterL::trigger()
{
    emit valuesChanged(d->values);
}
