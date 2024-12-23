/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medIntParameter.h>

#include <dtkLog>

class medIntParameterPrivate
{
public:
    int value;
    int min, max;
};

medIntParameter::medIntParameter(QString const& name,  QObject *parent)
    : medAbstractParameter(name, parent), d(new medIntParameterPrivate)
{
    d->value = 0;
    d->min = 0;
    d->max = 100;
    connect(this, &medIntParameter::valueChanged, this, &medIntParameter::triggered);
}

medIntParameter::~medIntParameter()
{

}

int medIntParameter::value() const
{
    return d->value;
}

bool medIntParameter::copyValueTo(medAbstractParameter & dest)
{
    bool bRes = typeid(dest) == typeid(*this);

    if (bRes)
    {
        dynamic_cast<medIntParameter*>(&dest)->setValue(value());
    }

    return bRes;
}

void medIntParameter::setValue(int value)
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

void medIntParameter::setRange(int min, int max)
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

int medIntParameter::minimum() const
{
    return d->min;
}

int medIntParameter::maximum() const
{
    return d->max;
}

void medIntParameter::trigger()
{
    emit valueChanged(d->value);
}

QVariantMap medIntParameter::toVariantMap() const
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

bool medIntParameter::fromVariantMap(QVariantMap const& pi_variantMap)
{
    bool bRes = true;

    bRes = bRes && pi_variantMap.contains("id");
    bRes = bRes && pi_variantMap.contains("caption");
    bRes = bRes && pi_variantMap.contains("description");
    bRes = bRes && pi_variantMap.contains("value");

    if (bRes)
    {
        bRes = bRes && pi_variantMap["value"].canConvert(QMetaType::Int);
        bRes = bRes && pi_variantMap["min"].canConvert(QMetaType::Int);
        bRes = bRes && pi_variantMap["max"].canConvert(QMetaType::Int);
        if (bRes)
        {
            setCaption(pi_variantMap["caption"].toString());
            setDescription(pi_variantMap["description"].toString());

            d->value = pi_variantMap["value"].toInt();
            d->min = pi_variantMap["min"].toInt();
            d->max = pi_variantMap["max"].toInt();
        }
    }

    return bRes;
}
