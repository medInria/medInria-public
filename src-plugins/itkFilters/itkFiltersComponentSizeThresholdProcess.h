/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractData.h>
#include "itkFiltersProcessBase.h"

#include "itkFiltersPluginExport.h"

class itkFiltersComponentSizeThresholdProcessPrivate;
class dtkAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersComponentSizeThresholdProcess : public itkFiltersProcessBase
{
    Q_OBJECT
    
public:
    itkFiltersComponentSizeThresholdProcess(itkFiltersComponentSizeThresholdProcess * parent = 0);
    itkFiltersComponentSizeThresholdProcess(const itkFiltersComponentSizeThresholdProcess& other);
    virtual ~itkFiltersComponentSizeThresholdProcess(void);

    static bool registered ( void );
    
public slots:

    void setParameter ( int  data, int channel );
    int update ( void );

private:
    DTK_DECLARE_PRIVATE(itkFiltersComponentSizeThresholdProcess)
};

dtkAbstractProcess * createitkFiltersComponentSizeThresholdProcess(void);

