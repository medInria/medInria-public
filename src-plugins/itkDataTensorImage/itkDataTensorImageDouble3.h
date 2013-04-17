/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractDataTypedImage.h>

#include "itkDataTensorImagePluginExport.h"

class itkDataTensorImageDouble3Private;

class ITKDATATENSORIMAGEPLUGIN_EXPORT itkDataTensorImageDouble3 : public medAbstractDataTypedImage<3,double>
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
    // derived from medAbstractDataImage
    int xDimension();
    int yDimension();
    int zDimension();

    /** TODO the image which is being returned right now is JUST A STUB to allow drag and drop */
    virtual QImage        &thumbnail() const;
    virtual QList<QImage> &thumbnails() const;

private:
    itkDataTensorImageDouble3Private *d;
};

dtkAbstractData *createItkDataTensorImageDouble3();


