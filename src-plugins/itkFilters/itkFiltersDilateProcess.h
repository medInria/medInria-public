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

class itkFiltersDilateProcessPrivate;
class medAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersDilateProcess : public itkFiltersProcessBase
{
    Q_OBJECT
    
public:
    itkFiltersDilateProcess(itkFiltersDilateProcess * parent = 0);
    virtual ~itkFiltersDilateProcess(void);

    static bool registered ( void );
    
public slots:
    int update ( void );

public:
    QList<medAbstractParameter*> parameters();

private:
    itkFiltersDilateProcessPrivate *d;
};

dtkAbstractProcess * createitkFiltersDilateProcess(void);
