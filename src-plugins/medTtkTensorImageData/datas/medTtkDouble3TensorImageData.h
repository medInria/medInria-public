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

#include <medTtkTensorImageExport.h>

class medTtkDouble3TensorImageDataPrivate;

class medTtkTensorImagePlugin_EXPORT medTtkDouble3TensorImageData : public medAbstractTypedDiffusionModelImageData<3,double>
{
    Q_OBJECT
    MED_DATA_INTERFACE("ITK Tensor Image (double3)",
                       "ITK Tensor Image (double3)")

public:
             medTtkDouble3TensorImageData();
    virtual ~medTtkDouble3TensorImageData();

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
    medTtkDouble3TensorImageDataPrivate *d;
};

medAbstractData *createmedTtkDouble3TensorImageData();


