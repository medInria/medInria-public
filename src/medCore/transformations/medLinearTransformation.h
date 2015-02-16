/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreExport.h>
#include <medAbstractTransformation.h>

#include <medAbstractImageData.h>

class medLinearTransformationPrivate;
class MEDCORE_EXPORT medLinearTransformation: public medAbstractTransformation
{
    Q_OBJECT

public:
    medLinearTransformation(QObject  *parent = NULL);
    ~medLinearTransformation();

public:
    virtual medAbstractData* parameter() const;
    virtual void setParameter(medAbstractData *parameter);

    virtual QMatrix4x4 matrix() const;
    virtual void setMatrix(QMatrix4x4 matrix);

private:
    medLinearTransformationPrivate* d;

    //using medAbstractTransformation::setParameter;
};
