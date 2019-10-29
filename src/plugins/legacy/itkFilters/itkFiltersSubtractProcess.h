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
class itkFiltersSubtractProcessPrivate;

class ITKFILTERSPLUGIN_EXPORT itkFiltersSubtractProcess : public itkFiltersProcessBase
{
    Q_OBJECT

public:
    static const double defaultSubtractValue;

    itkFiltersSubtractProcess(itkFiltersSubtractProcess *parent = nullptr);
    itkFiltersSubtractProcess(const itkFiltersSubtractProcess& other);
    virtual ~itkFiltersSubtractProcess();
    static bool registered();
    virtual QString description() const;
    
public slots:
    void setParameter(double data);
    int tryUpdate();
    
protected:
    template <class ImageType> int updateProcess(medAbstractData *inputData);

private:
    itkFiltersSubtractProcessPrivate *d;
};

dtkAbstractProcess * createitkFiltersSubtractProcess();
