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

class itkFiltersGaussianProcessPrivate;
class medAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersGaussianProcess : public itkFiltersProcessBase
{
    Q_OBJECT
    
public:
    itkFiltersGaussianProcess(itkFiltersGaussianProcess * parent = 0);
    virtual ~itkFiltersGaussianProcess(void);

    static bool registered ( void );
    
public slots:
    int update ( void );

public:
    QList<medAbstractParameter*> parameters();

private:
    itkFiltersGaussianProcessPrivate *d;
};

dtkAbstractProcess * createitkFiltersGaussianProcess(void);


