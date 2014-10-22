/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractProcess.h>
#include <medAbstractData.h>

#include <itkFiltersPluginExport.h>

class itkFiltersProcessBasePrivate;

class ITKFILTERSPLUGIN_EXPORT itkFiltersProcessBase : public dtkAbstractProcess
{
    Q_OBJECT
public:
    itkFiltersProcessBase(itkFiltersProcessBase * parent = 0);
    itkFiltersProcessBase(const itkFiltersProcessBase& other);
    virtual ~itkFiltersProcessBase(void);
    
public:
    itkFiltersProcessBase& operator = (const itkFiltersProcessBase& other);

    QString description ( void );
    
    void setInput ( medAbstractData *data );

    medAbstractData *output ( void );
    
    void emitProgress(int progress);
    
private:
    DTK_DECLARE_PRIVATE(itkFiltersProcessBase)
    
    using dtkAbstractProcess::description;
    using dtkAbstractProcess::setInput;
};



