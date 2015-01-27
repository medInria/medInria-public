/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medLinearTransformation.h>


class medLinearTransformationPrivate
{
public:
    QMatrix4x4 matrix;
    medAbstractData * data;
};




medLinearTransformation::medLinearTransformation(QObject  *parent): medAbstractTransformation(parent), d(new medLinearTransformationPrivate)
{
    d->data = NULL;
}

medLinearTransformation::~medLinearTransformation()
{
}

medAbstractData* medLinearTransformation::parameter() const
{
    return d->data;
}

void medLinearTransformation::setParameter(medAbstractData *parameter)
{
    d->data = parameter;
}

QMatrix4x4 medLinearTransformation::matrix() const
{
    return d->matrix;
}

void medLinearTransformation::setMatrix(QMatrix4x4 matrix)
{
    d->matrix = matrix;
}
