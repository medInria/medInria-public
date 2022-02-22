/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medVector2DParameter.h"
#include <QDomDocument>
#include <QDomElement>
#include <QVector2D>

class medVector2DParameterPrivate
{
public:
    QVector2D value;
};

medVector2DParameter::medVector2DParameter(QString name, QObject *parent)
    : medAbstractParameter(name, parent)
{
    connect(this, &medVector2DParameter::valueChanged, this, &medVector2DParameter::triggered);
}

medVector2DParameter::~medVector2DParameter()
{
}

void medVector2DParameter::setValue(const QVector2D &value)
{
    if (d->value != value)
    {
        d->value = value;
        emit valueChanged(value);
    }
}

QVector2D medVector2DParameter::value() const
{
    return d->value;
}

bool medVector2DParameter::copyValueTo(medAbstractParameter & dest)
{
    bool bRes = typeid(dest) == typeid(*this);

    if (bRes)
    {
        dynamic_cast<medVector2DParameter*>(&dest)->setValue(value());
    }

    return bRes;
}

QVariantMap medVector2DParameter::toVariantMap() const
{
    QVariantMap varMapRes;

    varMapRes.insert("id", id());
    varMapRes.insert("caption", caption());
    varMapRes.insert("description", description());

    varMapRes.insert("value", d->value);

    return varMapRes;
}

bool medVector2DParameter::fromVariantMap(QVariantMap const & pi_variantMap)
{
    bool bRes = true;

    bRes = bRes && pi_variantMap.contains("id");
    bRes = bRes && pi_variantMap.contains("caption");
    bRes = bRes && pi_variantMap.contains("description");
    bRes = bRes && pi_variantMap.contains("value");

    if (bRes)
    {
        bRes = bRes && pi_variantMap["value"].canConvert<QVector2D>();
        if (bRes)
        {
            setCaption(pi_variantMap["caption"].toString());
            setDescription(pi_variantMap["description"].toString());

            d->value = pi_variantMap["value"].value<QVector2D>();
        }
    }

    return bRes;
}

void medVector2DParameter::trigger()
{
    emit valueChanged(d->value);
}

QVector2D medVector2DParameter::fromString(QString value)
{
    QStringList tokens = value.split(" ");
    return QVector2D(tokens[0].toDouble(), tokens[1].toDouble());
}

void medVector2DParameter::toXMLNode(QDomDocument *doc, QDomElement *currentNode)
{
    QDomElement element = doc->createElement("name");
    element.appendChild(doc->createTextNode(id()));
    currentNode->appendChild(element);
    QDomElement type = doc->createElement("type");
    type.appendChild(doc->createTextNode("Vector2D"));
    currentNode->appendChild(type);
    element = doc->createElement("value");
    element.appendChild(doc->createTextNode(QString::number(value().x()) + " " + QString::number(value().y())));
    currentNode->appendChild(element);
}
