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

#include <itkDataSHImagePluginExport.h>

class itkDataSHImageFloat3Private;

class ITKDATASHIMAGEPLUGIN_EXPORT itkDataSHImageFloat3 : public medAbstractTypedImageData<3,float>
{
    Q_OBJECT

public:
             itkDataSHImageFloat3();
    virtual ~itkDataSHImageFloat3();

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
    itkDataSHImageFloat3Private *d;
};

medAbstractData *createItkDataSHImageFloat3();
