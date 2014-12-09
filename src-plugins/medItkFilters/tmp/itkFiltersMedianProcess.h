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

class itkFiltersMedianProcessPrivate;
class medAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersMedianProcess : public itkFiltersProcessBase
{
    Q_OBJECT
    
public:
    itkFiltersMedianProcess(itkFiltersMedianProcess * parent = 0);
    itkFiltersMedianProcess(const itkFiltersMedianProcess& other);
    virtual ~itkFiltersMedianProcess(void);

    static bool registered ( void );
    
public slots:

    int update ( void );

private:
    itkFiltersMedianProcessPrivate *d;
};


dtkAbstractProcess * createitkFiltersMedianProcess(void);



