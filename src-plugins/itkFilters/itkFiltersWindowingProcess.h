/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <itkFiltersProcessBase.h>

class itkFiltersWindowingProcessPrivate;
class medAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersWindowingProcess : public itkFiltersProcessBase
{
    Q_OBJECT
    
public:
    static const double defaultMinimumIntensityValue;
    static const double defaultMaximumIntensityValue;

    itkFiltersWindowingProcess(itkFiltersWindowingProcess * parent = 0);
    itkFiltersWindowingProcess(const itkFiltersWindowingProcess& other);
    virtual ~itkFiltersWindowingProcess(void);
    static bool registered ( void );

    virtual QString description(void) const;
    
public slots:

    void setParameter ( double  data, int channel );
    int tryUpdate();

protected:
    template <class ImageType> int updateProcess(medAbstractData* inputData);

private:
    itkFiltersWindowingProcessPrivate *d;
};

dtkAbstractProcess * createitkFiltersWindowingProcess(void);


