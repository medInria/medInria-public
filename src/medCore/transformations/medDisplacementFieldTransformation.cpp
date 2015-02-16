/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDisplacementFieldTransformation.h>

#include <medAbstractImageData.h>

#include <dtkCore/dtkSmartPointer.h>

class medDisplacementFieldTransformationPrivate
{
public:
    dtkSmartPointer<medAbstractImageData> parameter;
};


medDisplacementFieldTransformation::medDisplacementFieldTransformation(QObject *parent):medAbstractTransformation(parent),
    d(new medDisplacementFieldTransformationPrivate)
{
    d->parameter = NULL;
}

medDisplacementFieldTransformation::~medDisplacementFieldTransformation()
{
    delete d;
}

void medDisplacementFieldTransformation::setParameter(medAbstractData *parameter)
{
    medAbstractImageData *imData = dynamic_cast<medAbstractImageData *>(parameter);
    if(!imData)
        return;

    qDebug() << imData->identifier();

    if(imData->dimension() != 3)
    {
        qDebug() << "Set wrong parameter to medDisplacementFieldTransformation";
        return;
    }

    if(!imData->identifier().contains("vector", Qt::CaseInsensitive))
    {
        qDebug() << "Set wrong parameter to medDisplacementFieldTransformation";
        return;
    }

    d->parameter = imData;
}

medAbstractImageData* medDisplacementFieldTransformation::parameter() const
{
    return d->parameter;
}
