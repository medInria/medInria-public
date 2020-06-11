/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medVector3DParameterL.h"
#include <QVector3D>

medVector3DParameterL::medVector3DParameterL(QString name, QObject *parent)
    : medAbstractVector3DParameterL(name, parent)
{
}

medVector3DParameterL::~medVector3DParameterL()
{
}

QWidget* medVector3DParameterL::getWidget()
{
    return nullptr;
}

void medVector3DParameterL::updateInternWigets()
{
}
