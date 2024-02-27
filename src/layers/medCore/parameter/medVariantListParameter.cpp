/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medVariantListParameter.h"

#include <QPair>
#include <QDebug>


class medCompositeParameterPrivate
{
public:

    QHash<QString, QVariant> variants;
    QHash<QString, QPair <QVariant, QVariant> > ranges;
    QHash<QString, QVariant> steps;

    ~medCompositeParameterPrivate()
    {
        variants.clear();
    }
};

medVariantListParameter::medVariantListParameter(QString name, QObject* parent):
        medAbstractParameter(name, parent),
        d(new medCompositeParameterPrivate)
{

}

medVariantListParameter::~medVariantListParameter()
{
    delete d;
}

void medVariantListParameter::setValues(const QHash<QString, QVariant> value)
{
    QHash<QString, QVariant>::const_iterator valuesIterator = value.constBegin();
    QHash<QString, QVariant> valuesAdded;

    while (valuesIterator != value.constEnd())
    {
        QString key = valuesIterator.key();
        if (!setValue(key, valuesIterator.value()))
        {
            valuesAdded[key] = valuesIterator.value();
        }
        valuesIterator++;
    }

    emit valuesChanged(valuesAdded);
}

bool medVariantListParameter::setValue(QString const & name, QVariant const & value)
{
    bool bValueUpdated = false;

    if(d->variants.contains(name) && d->variants[name].type() == value.type())
    {
        if (d->variants[name].type() == QVariant::Double)
        {
            double previousValue = d->variants[name].toDouble();

            if (value.toDouble() < d->ranges.value(name).first.toDouble())
                d->variants[name] = d->ranges.value(name).first;
            else if (value.toDouble() > d->ranges.value(name).second.toDouble())
                d->variants[name] = d->ranges.value(name).second;
            else
                d->variants[name] = value;

            if (previousValue != d->variants[name])
                bValueUpdated = true;
        }
        else if (d->variants[name].type() == QVariant::Int)
        {
            int previousValue = d->variants[name].toInt();

            if (value.toInt() < d->ranges.value(name).first.toInt())
                d->variants[name] = d->ranges.value(name).first;
            else if (value.toInt() > d->ranges.value(name).second.toInt())
                d->variants[name] = d->ranges.value(name).second;
            else
                d->variants[name] = value;

            if (previousValue != d->variants[name])
                bValueUpdated = true;
        }
        else
        {
            if (!(d->variants[name] == value))
            {
                d->variants[name] = value;
                bValueUpdated = true;
            }
        }
    }
    else
    {
        qDebug() << this->id() << ": add unknown variant" << name << "with variant" << value.toString();
    }

    if (bValueUpdated)
    {
        emit valueChanged(name, value);
    }

    return bValueUpdated;
}

QList<QVariant> medVariantListParameter::values() const
{
    return d->variants.values();
}

QVariant medVariantListParameter::value(const QString key) const
{
    QVariant oRes;
    if (d->variants.contains(key))
    {
        oRes = d->variants[key];
    }
    return d->variants[key];
}

void medVariantListParameter::addVariant(QString name, QVariant variant, QVariant min, QVariant max, QVariant step)
{
    d->variants.insert(name, variant);
    d->ranges.insert(name, QPair<QVariant, QVariant>(min, max));
    emit variantAdded(name, variant);
}

void medVariantListParameter::delVariant(QString name)
{
    d->variants.remove(name);
    d->ranges.remove(name);
    emit variantRemoved(name);
}

QList<QPair<QVariant, QVariant> > medVariantListParameter::ranges() const
{
    return d->ranges.values();
}

QList<QVariant> medVariantListParameter::steps() const
{
    return d->steps.values();
}

void medVariantListParameter::trigger()
{
    emit valuesChanged(d->variants);
}

bool medVariantListParameter::copyValueTo(medAbstractParameter & dest)
{
    bool bRes = typeid(dest) == typeid(*this);

    if (bRes)
    {
        medVariantListParameter *pDst = dynamic_cast<medVariantListParameter*>(&dest);
        auto keysList = d->variants.keys();
        for (auto key : keysList)
        {
            auto &var   = d->variants[key];
            auto &range = d->ranges[key];
            pDst->addVariant(key, var, range.first, range.second);
        }
    }

    return bRes;
}

QVariantMap medVariantListParameter::toVariantMap() const
{
    return QVariantMap(); //TODO
}

bool medVariantListParameter::fromVariantMap(QVariantMap const & pi_variantMap)
{
    return false; //TODO
}
