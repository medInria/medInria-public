/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <itkFiltersProcessBase.h>

class medAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersInvertProcess : public itkFiltersProcessBase
{
    Q_OBJECT
    
public:
    itkFiltersInvertProcess(itkFiltersInvertProcess *parent = nullptr);
    static bool registered();
    virtual QString description() const;
    
public slots:
    int tryUpdate();

protected:
    template <class ImageType> int updateProcess(medAbstractData *inputData);
};

dtkAbstractProcess * createitkFiltersInvertProcess();
