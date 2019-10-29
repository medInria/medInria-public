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

class itkFiltersMedianProcessPrivate;
class medAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersMedianProcess : public itkFiltersProcessBase
{
    Q_OBJECT
    
public:
    static const double defaultMedianSize;

    itkFiltersMedianProcess(itkFiltersMedianProcess *parent = nullptr);

    static bool registered();
    virtual QString description() const;
    
public slots:
    void setParameter(double data);
    int tryUpdate();

protected:
    template <class ImageType> int updateProcess(medAbstractData *inputData);

private:
    itkFiltersMedianProcessPrivate *d;

};

dtkAbstractProcess * createitkFiltersMedianProcess();
