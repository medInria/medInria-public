/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCore/medAbstractDataTypedImage.h>

#include <itkDataSHImagePluginExport.h>

class itkDataSHImageFloat3Private;

class ITKDATASHIMAGEPLUGIN_EXPORT itkDataSHImageFloat3 : public medAbstractDataTypedImage<3,float>
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
    // derived from medAbstractDataImage
    int xDimension();
    int yDimension();
    int zDimension();
 
private:
    itkDataSHImageFloat3Private *d;
};

dtkAbstractData *createItkDataSHImageFloat3();
