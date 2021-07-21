/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDoubleParameter.h>

#include <dtkLog>

class medDoubleParameterPrivate
{
public:
    double value;
    double min, max;
};

medDoubleParameter::medDoubleParameter(QString const& name,  QObject *parent)
    : medAbstractParameter(name, parent), d(new medDoubleParameterPrivate)
{
    d->value = 0.0;
    d->min = 0.0;
    d->max = 100.0;
}

medDoubleParameter::~medDoubleParameter()
{

}

double medDoubleParameter::value() const
{
    return d->value;
}

void medDoubleParameter::setValue(double value)
{
    if(value != d->value)
    {
        if(value < d->min)
            d->value = d->min;
        else if(value > d->max)
            d->value = d->max;
        else d->value = value;

        d->value = value;
        emit valueChanged(d->value);
    }
}

void medDoubleParameter::setRange(double min, double max)
{
    if(min >= max)
    {
        qDebug() << "attempt to set invalid range to "
                   << this->caption() << this->id() << this;
    }
    else
    {
        d->min = min;
        d->max = max;
        emit rangeChanged(min, max);
    }
}

double medDoubleParameter::minimum() const
{
    return d->min;
}

double medDoubleParameter::maximum() const
{
    return d->max;
}

void medDoubleParameter::trigger()
{
    emit valueChanged(d->value);
}

QVariantMap medDoubleParameter::toVariantMap() const
{
    QVariantMap varMapRes;

    varMapRes.insert("id", id());
    varMapRes.insert("caption", caption());
    varMapRes.insert("description", description());

    varMapRes.insert("value", d->value);
    varMapRes.insert("min", d->min);
    varMapRes.insert("max", d->max);

    return varMapRes;
}

bool medDoubleParameter::fromVariantMap(QVariantMap & pi_variantMap)
{
    bool bRes = true;

    bRes &= pi_variantMap.contains("id");
    bRes &= pi_variantMap.contains("caption");
    bRes &= pi_variantMap.contains("description");
    bRes &= pi_variantMap.contains("value");

    if (bRes)
    {
        bRes &= pi_variantMap["value"].canConvert(QMetaType::Double);
        bRes &= pi_variantMap["min"].canConvert(QMetaType::Double);
        bRes &= pi_variantMap["max"].canConvert(QMetaType::Double);
        if (bRes)
        {
            setCaption(pi_variantMap["caption"].toString());
            setDescription(pi_variantMap["description"].toString());

            d->value = pi_variantMap["value"].toDouble();
            d->min = pi_variantMap["min"].toDouble();
            d->max = pi_variantMap["max"].toDouble();
        }
    }

    return bRes;
}
