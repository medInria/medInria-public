/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
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
    static const int defaultOutsideValue;
    static const bool defaultComparisonOperator;

    itkFiltersThresholdingProcess(itkFiltersThresholdingProcess * parent = 0);
    itkFiltersThresholdingProcess(const itkFiltersThresholdingProcess& other);
    virtual ~itkFiltersThresholdingProcess(void);
    static bool registered ( void );

    virtual QString description(void) const;
    
public slots:

    void setParameter ( double  data, int channel );
    int tryUpdate();

protected:
    template <class ImageType> int updateProcess(medAbstractData* inputData);

private:
    itkFiltersThresholdingProcessPrivate *d;
};

dtkAbstractProcess * createitkFiltersThresholdingProcess(void);


