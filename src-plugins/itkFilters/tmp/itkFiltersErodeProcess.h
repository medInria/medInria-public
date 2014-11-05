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

class itkFiltersErodeProcessPrivate;
class medAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersErodeProcess : public itkFiltersProcessBase
{
    Q_OBJECT
    
public:
    itkFiltersErodeProcess(itkFiltersErodeProcess * parent = 0);
    virtual ~itkFiltersErodeProcess(void);

    static bool registered ( void );
    
public slots:
    int update ( void );

public:
    QList<medAbstractParameter*> parameters();

private:
    itkFiltersErodeProcessPrivate *d;
};

dtkAbstractProcess * createitkFiltersErodeProcess(void);
