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

#include <medItkSHImageExport.h>

class medItkFloat3SHImageDataPrivate;

class MEDITKSHIMAGEPLUGIN_EXPORT medItkFloat3SHImageData : public medAbstractTypedDiffusionModelImageData<3,float>
{
    Q_OBJECT
    MED_DATA_INTERFACE("ITK Spherical Harmonics Image (float3)",
                       "ITK Spherical Harmonics Image (float3)")
public:
             medItkFloat3SHImageData();
    virtual ~medItkFloat3SHImageData();

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
    medItkFloat3SHImageDataPrivate *d;
};
