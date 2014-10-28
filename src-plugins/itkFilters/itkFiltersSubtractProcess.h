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

class medAbstractData;
class itkFiltersSubtractProcessPrivate;

class ITKFILTERSPLUGIN_EXPORT itkFiltersSubtractProcess : public itkFiltersProcessBase
{
    Q_OBJECT
public:
    itkFiltersSubtractProcess(itkFiltersSubtractProcess * parent = 0);
    virtual ~itkFiltersSubtractProcess(void);
    
    static bool registered(void);
    
public slots:
    int update(void);

public:
    virtual void setInputImage ( medAbstractData *data );


public:
    QList<medAbstractParameter*> parameters();
    
private:
    itkFiltersSubtractProcessPrivate *d;
};


dtkAbstractProcess * createitkFiltersSubtractProcess(void);


