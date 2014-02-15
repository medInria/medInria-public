/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractTypedImageData.h>

#include <itkDataTensorImagePluginExport.h>

class itkDataTensorImageDouble3Private;

class ITKDATATENSORIMAGEPLUGIN_EXPORT itkDataTensorImageDouble3 : public medAbstractTypedImageData<3,double>
{
    Q_OBJECT

public:
             itkDataTensorImageDouble3();
    virtual ~itkDataTensorImageDouble3();

    virtual QString description() const;
    virtual QString identifier() const;

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
    itkDataTensorImageDouble3Private *d;
};

medAbstractData *createItkDataTensorImageDouble3();


