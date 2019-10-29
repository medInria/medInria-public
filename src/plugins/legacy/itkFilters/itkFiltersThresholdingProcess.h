/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "itkFiltersProcessBase.h"

class itkFiltersThresholdingProcessPrivate;
class medAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersThresholdingProcess : public itkFiltersProcessBase
{
    Q_OBJECT
    
public:
    static const double defaultThreshold;
    static const double defaultUpper;
    static const double defaultLower;
    static const int defaultOutsideValue;
    static const int upperButtonId;
    static const int lowerButtonId;
    static const int outsideButtonId;

    itkFiltersThresholdingProcess(itkFiltersThresholdingProcess *parent = nullptr);
    itkFiltersThresholdingProcess(const itkFiltersThresholdingProcess& other);
    virtual ~itkFiltersThresholdingProcess();
    static bool registered();

    virtual QString description() const;
    
public slots:
    void setParameter(int data);
    void setParameter(double  data, int channel);
    int tryUpdate();

protected:
    template <class ImageType> int updateProcess(medAbstractData *inputData);

private:
    itkFiltersThresholdingProcessPrivate *d;
};

dtkAbstractProcess * createitkFiltersThresholdingProcess();
