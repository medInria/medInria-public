/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medVector3DParameter.h"
#include <QVector3D>

class medVector3DParameterPrivate
{
public:

};

medVector3DParameter::medVector3DParameter(QString name, QObject *parent)
    : medAbstractVector3DParameter(name, parent), d(new medVector3DParameterPrivate)
{
}

medVector3DParameter::~medVector3DParameter()
{
    delete d;
}

QWidget* medVector3DParameter::getWidget()
{
    return 0;
}

void medVector3DParameter::updateInternWigets()
{
    return;
}
