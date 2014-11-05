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

class itkFiltersDivideProcessPrivate;
class medAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersDivideProcess : public itkFiltersProcessBase
{
    Q_OBJECT
    
public:
    itkFiltersDivideProcess(itkFiltersDivideProcess * parent = 0);
    virtual ~itkFiltersDivideProcess(void);

    static bool registered ( void );
    
public slots:
    int update ( void );

public:
    QList<medAbstractParameter*> parameters();

private:
    itkFiltersDivideProcessPrivate* d;
};

dtkAbstractProcess * createitkFiltersDivideProcess(void);


