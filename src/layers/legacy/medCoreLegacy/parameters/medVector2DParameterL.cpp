/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medVector2DParameterL.h"
#include <QVector2D>

class medVector2DParameterLPrivate
{
public:

};

medVector2DParameterL::medVector2DParameterL(QString name, QObject *parent)
    : medAbstractVector2DParameterL(name, parent), d(new medVector2DParameterLPrivate)
{
}

medVector2DParameterL::~medVector2DParameterL()
{
    delete d;
}

QWidget* medVector2DParameterL::getWidget()
{
    return 0;
}

void medVector2DParameterL::updateInternWigets()
{
    return;
}
