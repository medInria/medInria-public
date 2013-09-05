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
#include <dtkCore/dtkAbstractData.h>

#include "itkFiltersPluginExport.h"

class itkFiltersMultiplyProcessPrivate;
class dtkAbstractData;

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
    int update ( void );

private:
    DTK_DECLARE_PRIVATE(itkFiltersMultiplyProcess)
};


dtkAbstractProcess * createitkFiltersMultiplyProcess(void);


