/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractTypedDiffusionModelImageData.h>

#include <itkDataTensorImagePluginExport.h>

class itkDataTensorImageFloat3Private;

class ITKDATATENSORIMAGEPLUGIN_EXPORT itkDataTensorImageFloat3 : public medAbstractTypedDiffusionModelImageData<3,float>
{
    Q_OBJECT
    MED_DATA_INTERFACE("ITK Tensor Image (float3)",
                       "ITK Tensor Image (float3)")
public:
             itkDataTensorImageFloat3();
    virtual ~itkDataTensorImageFloat3();

    static bool registered();

public slots:
    // derived from dtkAbstractData
    void *output();
    void *data();
    void setData(void* data);

public:
    // derived from medAbstractImageData
    int xDimension();
    int yDimension();
    int zDimension();

private:
    itkDataTensorImageFloat3Private *d;
};


