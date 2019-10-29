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

class itkFiltersGaussianProcessPrivate;
class medAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersGaussianProcess : public itkFiltersProcessBase
{
    Q_OBJECT
    
public:
    static const double defaultSigma;

    itkFiltersGaussianProcess(itkFiltersGaussianProcess *parent = nullptr);
    itkFiltersGaussianProcess(const itkFiltersGaussianProcess& other);
    virtual ~itkFiltersGaussianProcess();
    static bool registered();
    virtual QString description() const;
    
public slots:
    void setParameter(double data);
    int tryUpdate();

protected:
    template <class ImageType> int updateProcess(medAbstractData *inputData);

private:
    itkFiltersGaussianProcessPrivate *d;
};

dtkAbstractProcess * createitkFiltersGaussianProcess();
