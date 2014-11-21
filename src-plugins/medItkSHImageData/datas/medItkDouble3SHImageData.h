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

class medItkDouble3SHImageDataPrivate;

class MEDITKSHIMAGEPLUGIN_EXPORT medItkDouble3SHImageData : public medAbstractTypedDiffusionModelImageData<3,double>
{
    Q_OBJECT
    MED_DATA_INTERFACE("ITK Spherical Harmonics Image (double3)",
                       "ITK Spherical Harmonics Image (double3)")
public:
             medItkDouble3SHImageData();
    virtual ~medItkDouble3SHImageData();

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
    medItkDouble3SHImageDataPrivate *d;
};
