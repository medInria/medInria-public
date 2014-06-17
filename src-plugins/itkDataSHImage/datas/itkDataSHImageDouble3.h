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
#include <itkDataSHImagePluginExport.h>

class itkDataSHImageDouble3Private;

class ITKDATASHIMAGEPLUGIN_EXPORT itkDataSHImageDouble3 : public medAbstractTypedDiffusionModelImageData<3,double>
{
    Q_OBJECT

public:
             itkDataSHImageDouble3();
    virtual ~itkDataSHImageDouble3();

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
    itkDataSHImageDouble3Private *d;
};

medAbstractData *createItkDataSHImageDouble3();
