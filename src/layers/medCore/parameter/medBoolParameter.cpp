/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medBoolParameter.h>

class medBoolParameterPrivate
{
public:
    bool value;
};

medBoolParameter::medBoolParameter(QString const& name,  QObject *parent)
    : medAbstractParameter(name, parent), d(new medBoolParameterPrivate)
{
    connect(this, &medBoolParameter::valueChanged, this, &medBoolParameter::triggered);
}

medBoolParameter::~medBoolParameter()
{

}

bool medBoolParameter::value() const
{
    return d->value;
}

bool medBoolParameter::copyValueTo(medAbstractParameter & dest)
{
    bool bRes = typeid(dest) == typeid(*this);

    if (bRes)
    {
        dynamic_cast<medBoolParameter*>(&dest)->setValue(value());
    }

    return bRes;
}

void medBoolParameter::setValue(bool value)
{
    if(value != d->value)
    {
        d->value = value;
        emit valueChanged(d->value);
    }
}

void medBoolParameter::trigger()
{
    emit valueChanged(d->value);
}

QVariantMap medBoolParameter::toVariantMap() const
{
    QVariantMap varMapRes;

    varMapRes.insert("id",id());
    varMapRes.insert("caption", caption());
    varMapRes.insert("description", description());

    varMapRes.insert("value", d->value);
    
    return varMapRes;
}

bool medBoolParameter::fromVariantMap(QVariantMap const& pi_variantMap)
{
    bool bRes = true;

    bRes = bRes && pi_variantMap.contains("id");
    bRes = bRes && pi_variantMap.contains("caption");
    bRes = bRes && pi_variantMap.contains("description");
    bRes = bRes && pi_variantMap.contains("value");

    if (bRes)
    {
        bRes = pi_variantMap["value"].canConvert(QMetaType::Bool);
        if (bRes)
        {
            setCaption(pi_variantMap["caption"].toString());
            setDescription(pi_variantMap["description"].toString());

            d->value = pi_variantMap["value"].toBool();
        }
    }

    return bRes;
}