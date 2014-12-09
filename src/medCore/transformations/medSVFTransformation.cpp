/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSVFTransformation.h>

#include <medAbstractImageData.h>

class medSVFTransformationPrivate
{
public:
    medAbstractImageData *parameter;
};


medSVFTransformation::medSVFTransformation(QObject *parent):
    d(new medSVFTransformationPrivate)
{
    d->parameter = NULL;
}

medSVFTransformation::~medSVFTransformation()
{
    delete d;
}

void medSVFTransformation::setParameter(medAbstractImageData *parameter)
{
    if(!parameter)
        return;

    if(parameter->dimension() != 3)
    {
        qDebug() << "Set wrong parameter to medSVFTransformation";
        return;
    }

    if(!parameter->identifier().contains("vector", Qt::CaseInsensitive))
    {
        qDebug() << "Set wrong parameter to medSVFTransformation";
        return;
    }

    d->parameter = parameter;
}

medAbstractImageData* medSVFTransformation::parameter() const
{
    return d->parameter;
}
