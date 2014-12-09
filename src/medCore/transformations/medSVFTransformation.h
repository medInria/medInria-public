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

class medSVFTransformationPrivate;
class MEDCORE_EXPORT medSVFTransformation: public medAbstractTransformation
{
    Q_OBJECT

public:
    medSVFTransformation(QObject  *parent = NULL);
    ~medSVFTransformation();

public:
    virtual medAbstractImageData* parameter() const;
    virtual void setParameter(medAbstractImageData *parameter);

private:
    medSVFTransformationPrivate* d;

    using medAbstractTransformation::setParameter;
};
