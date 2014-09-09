/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medVector2DParameter.h"
#include <QVector2D>

class medVector2DParameterPrivate
{
public:

};

medVector2DParameter::medVector2DParameter(QString name, QObject *parent)
    : medAbstractVector2DParameter(name, parent), d(new medVector2DParameterPrivate)
{
}

medVector2DParameter::~medVector2DParameter()
{
    delete d;
}

QWidget* medVector2DParameter::getWidget()
{
    return 0;
}

void medVector2DParameter::updateInternWigets()
{
    return;
}
