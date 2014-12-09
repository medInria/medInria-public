/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractData.h>
#include <itkFiltersProcessBase.h>

#include <itkFiltersPluginExport.h>

class itkFiltersOpenProcessPrivate;
class medAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersOpenProcess : public itkFiltersProcessBase
{
    Q_OBJECT
    
public:
    itkFiltersOpenProcess(itkFiltersOpenProcess * parent = 0);
    virtual ~itkFiltersOpenProcess(void);

    static bool registered ( void );
    
public slots:
    int update ( void );

public:
    QList<medAbstractParameter*> parameters();

private:
    itkFiltersOpenProcessPrivate *d;
};

dtkAbstractProcess * createitkFiltersOpenProcess(void);
