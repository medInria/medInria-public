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

class itkFiltersNormalizeProcessPrivate;
class medAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersNormalizeProcess : public itkFiltersProcessBase
{
    Q_OBJECT
    
public:
    itkFiltersNormalizeProcess(itkFiltersNormalizeProcess * parent = 0);
    itkFiltersNormalizeProcess(const itkFiltersNormalizeProcess& other);
    virtual ~itkFiltersNormalizeProcess(void);

    static bool registered ( void );
    
public slots:

    int update ( void );

private:
    itkFiltersNormalizeProcessPrivate *d;
};

dtkAbstractProcess * createitkFiltersNormalizeProcess(void);



