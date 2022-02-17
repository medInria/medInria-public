/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDataListParameter.h>
#include <medDataIndex.h>

class medDataListParameterPrivate
{
public:
    QList <medDataIndex> values;
};

medDataListParameter::medDataListParameter(QString name, QObject* parent):
    medAbstractParameter(name, parent),
    d(new medDataListParameterPrivate)
{
    d->values.clear();
    connect(this, &medDataListParameter::valuesChanged, this, &medDataListParameter::triggered);
}

medDataListParameter::~medDataListParameter()
{
    delete d;
}

void medDataListParameter::setValues(QList<medDataIndex> values)
{
    d->values = values;
    emit valuesChanged(d->values);
}

void medDataListParameter::clear()
{
    d->values.clear();
    emit valuesChanged(d->values);
}

QList<medDataIndex> &medDataListParameter::values() const
{
    return d->values;
}

bool medDataListParameter::copyValueTo(medAbstractParameter & dest)
{
    bool bRes = typeid(dest) == typeid(*this);

    if (bRes)
    {
        dynamic_cast<medDataListParameter*>(&dest)->setValues(values());
    }

    return bRes;
}

void medDataListParameter::trigger()
{
    emit valuesChanged(d->values);
}

QVariantMap medDataListParameter::toVariantMap() const
{
    return QVariantMap(); //TODO
}

bool medDataListParameter::fromVariantMap(QVariantMap const & pi_variantMap)
{
    bool bRes = false;
}
