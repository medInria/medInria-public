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

class medDisplacementFieldTransformationPrivate;
class medDisplacementFieldTransformation : public medAbstractTransformation
{
    Q_OBJECT

public:
    medDisplacementFieldTransformation(QObject  *parent = NULL);
    ~medDisplacementFieldTransformation();

public:
    virtual medAbstractImageData* parameter() const;
    virtual void setParameter(medAbstractData *parameter);

private:
    medDisplacementFieldTransformationPrivate* d;

    //using medAbstractTransformation::setParameter;
};
