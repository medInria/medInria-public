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

}

QVariantMap medTriggerParameter::toVariantMap() const
{
    return QVariantMap(); //TODO
}

bool medTriggerParameter::fromVariantMap(QVariantMap const & pi_variantMap)
{
    return false; //TODO
}

