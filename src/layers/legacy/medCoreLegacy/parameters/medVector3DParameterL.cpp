/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medVector3DParameterL.h"
#include <QVector3D>

class medVector3DParameterLPrivate
{
public:

};

medVector3DParameterL::medVector3DParameterL(QString name, QObject *parent)
    : medAbstractVector3DParameterL(name, parent), d(new medVector3DParameterLPrivate)
{
}

medVector3DParameterL::~medVector3DParameterL()
{
    delete d;
}

QWidget* medVector3DParameterL::getWidget()
{
    return 0;
}

void medVector3DParameterL::updateInternWigets()
{
    return;
}
