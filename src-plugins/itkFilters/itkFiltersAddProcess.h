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

class itkFiltersAddProcessPrivate;
class medAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersAddProcess : public itkFiltersProcessBase
{
    Q_OBJECT
    
public:
    itkFiltersAddProcess(itkFiltersAddProcess * parent = 0);
    virtual ~itkFiltersAddProcess(void);

    static bool registered ( void );
    
public slots:
    int update ( void );

public:
    virtual void setInputImage ( medAbstractData *data );

public:
    QList<medAbstractParameter*> parameters();

private:
    itkFiltersAddProcessPrivate* d;
};

dtkAbstractProcess * createitkFiltersAddProcess(void);

