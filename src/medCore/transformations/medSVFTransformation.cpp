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

#include <dtkCore/dtkSmartPointer.h>


class medSVFTransformationPrivate
{
public:
    dtkSmartPointer<medAbstractImageData> parameter;
};


medSVFTransformation::medSVFTransformation(QObject *parent):medAbstractTransformation(parent),
    d(new medSVFTransformationPrivate)
{
    d->parameter = NULL;
}

medSVFTransformation::~medSVFTransformation()
{
    delete d;
}

void medSVFTransformation::setParameter(medAbstractData *parameter)
{    
    medAbstractImageData *imData = dynamic_cast<medAbstractImageData *>(parameter);
    if(!imData)
        return;

    if(imData->dimension() != 3)
    {
        qDebug() << "Set wrong parameter to medSVFTransformation";
        return;
    }

    if(!imData->identifier().contains("vector", Qt::CaseInsensitive))
    {
        qDebug() << "Set wrong parameter to medSVFTransformation";
        return;
    }

    d->parameter = imData;
}

medAbstractImageData* medSVFTransformation::parameter() const
{
    return d->parameter;
}
