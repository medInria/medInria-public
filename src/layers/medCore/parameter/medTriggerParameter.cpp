/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medTriggerParameter.h>

#include <QIcon>
#include <QPushButton>



medTriggerParameter::medTriggerParameter(QString name, QObject *parent): medAbstractParameter(name, parent)
{

}

medTriggerParameter::~medTriggerParameter()
{
}

void medTriggerParameter::trigger()
{
    emit triggered();
}

QVariantMap medTriggerParameter::toVariantMap() const
{
    QVariantMap varMapRes;

    varMapRes.insert("id",id());
    varMapRes.insert("caption", caption());
    varMapRes.insert("description", description());

    return varMapRes;
}

bool medTriggerParameter::fromVariantMap(QVariantMap const & pi_variantMap)
{
    bool bRes = true;

    bRes = bRes && pi_variantMap.contains("id");
    bRes = bRes && pi_variantMap.contains("caption");
    bRes = bRes && pi_variantMap.contains("description");

    if (bRes)
    {
        setCaption(pi_variantMap["caption"].toString());
        setDescription(pi_variantMap["description"].toString());
    }

    return bRes;
}

