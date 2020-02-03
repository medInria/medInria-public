/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medVector2DParameterL.h"
#include <QVector2D>

medVector2DParameterL::medVector2DParameterL(QString name, QObject *parent)
    : medAbstractVector2DParameterL(name, parent)
{
}

medVector2DParameterL::~medVector2DParameterL()
{
}

QWidget* medVector2DParameterL::getWidget()
{
    return 0;
}

void medVector2DParameterL::updateInternWigets()
{
    return;
}
