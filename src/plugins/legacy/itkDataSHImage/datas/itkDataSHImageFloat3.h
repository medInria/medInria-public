/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractTypedDiffusionModelImageData.h>

#include <itkDataSHImagePluginExport.h>

class itkDataSHImageFloat3Private;

class ITKDATASHIMAGEPLUGIN_EXPORT itkDataSHImageFloat3 : public medAbstractTypedDiffusionModelImageData<3,float>
{
    Q_OBJECT
    MED_DATA_INTERFACE("ITK Spherical Harmonics Image (float3)",
                       "ITK Spherical Harmonics Image (float3)")
public:
             itkDataSHImageFloat3();
    virtual ~itkDataSHImageFloat3();

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
    itkDataSHImageFloat3Private *d;
};
