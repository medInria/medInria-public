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
#include <medAbstractData.h>

#include <itkFiltersPluginExport.h>

class itkFiltersMultiplyProcessPrivate;
class medAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersMultiplyProcess : public itkFiltersProcessBase
{
    Q_OBJECT
    
public:
    itkFiltersMultiplyProcess(itkFiltersMultiplyProcess * parent = 0);
    itkFiltersMultiplyProcess(const itkFiltersMultiplyProcess& other);
    virtual ~itkFiltersMultiplyProcess(void);

    static bool registered ( void );
    
public slots:

    void setParameter ( double  data, int channel );
    int tryUpdate();

private:
    DTK_DECLARE_PRIVATE(itkFiltersMultiplyProcess)
};


dtkAbstractProcess * createitkFiltersMultiplyProcess(void);


