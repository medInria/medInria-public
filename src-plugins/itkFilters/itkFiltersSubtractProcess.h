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

class medAbstractData;
class itkFiltersSubtractProcessPrivate;

class ITKFILTERSPLUGIN_EXPORT itkFiltersSubtractProcess : public itkFiltersProcessBase
{
    Q_OBJECT
public:
    itkFiltersSubtractProcess(itkFiltersSubtractProcess * parent = 0);
    virtual ~itkFiltersSubtractProcess(void);
    static bool registered(void);
    
public slots:
    void setParameter(double data, int channel);
    int tryUpdate();
    
protected:
    template <class PixelType> int updateProcess();

private:
    itkFiltersSubtractProcessPrivate *d;
};

dtkAbstractProcess * createitkFiltersSubtractProcess(void);


