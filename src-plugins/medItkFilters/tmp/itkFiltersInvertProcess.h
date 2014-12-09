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

class itkFiltersInvertProcessPrivate;
class medAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersInvertProcess : public itkFiltersProcessBase
{
    Q_OBJECT
    
public:
    itkFiltersInvertProcess(itkFiltersInvertProcess * parent = 0);
    virtual ~itkFiltersInvertProcess(void);

    static bool registered ( void );
    
public slots:

    int update ( void );

private:
    itkFiltersInvertProcessPrivate *d;
};


dtkAbstractProcess * createitkFiltersInvertProcess(void);


