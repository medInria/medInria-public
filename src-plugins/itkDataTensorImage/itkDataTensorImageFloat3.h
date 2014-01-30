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

#include <itkDataTensorImagePluginExport.h>

class itkDataTensorImageFloat3Private;

class ITKDATATENSORIMAGEPLUGIN_EXPORT itkDataTensorImageFloat3 : public medAbstractDataTypedImage<3,float>
{
    Q_OBJECT

public:
             itkDataTensorImageFloat3();
    virtual ~itkDataTensorImageFloat3();

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
    virtual QImage        &thumbnail();
    virtual QList<QImage> &thumbnails();

private:
    itkDataTensorImageFloat3Private *d;
};

medAbstractData *createItkDataTensorImageFloat3();


