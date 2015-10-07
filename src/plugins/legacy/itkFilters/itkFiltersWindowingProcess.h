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

class itkFiltersWindowingProcessPrivate;
class medAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersWindowingProcess : public itkFiltersProcessBase
{
    Q_OBJECT
    
public:
    itkFiltersWindowingProcess(itkFiltersWindowingProcess * parent = 0);
    itkFiltersWindowingProcess(const itkFiltersWindowingProcess& other);
    virtual ~itkFiltersWindowingProcess(void);

    static bool registered ( void );
    
public slots:

    void setParameter ( double  data, int channel );
    int update ( void );

private:
    DTK_DECLARE_PRIVATE(itkFiltersWindowingProcess)
};


dtkAbstractProcess * createitkFiltersWindowingProcess(void);


