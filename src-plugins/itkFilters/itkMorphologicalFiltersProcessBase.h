/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractProcess.h>
#include <medAbstractData.h>

#include <itkFiltersPluginExport.h>

class itkMorphologicalFiltersProcessBasePrivate;

class ITKFILTERSPLUGIN_EXPORT itkMorphologicalFiltersProcessBase : public medAbstractProcess
{
    Q_OBJECT
public:
    itkMorphologicalFiltersProcessBase(itkMorphologicalFiltersProcessBase * parent = 0);
    itkMorphologicalFiltersProcessBase(const itkMorphologicalFiltersProcessBase& other);
    virtual ~itkMorphologicalFiltersProcessBase(void);
    
public:
    itkMorphologicalFiltersProcessBase& operator = (const itkMorphologicalFiltersProcessBase& other);

    QString description ( void );
    
    void setInput ( medAbstractData *data, int channel = 0 );
	
	void setParameter(double data, int channel);

    medAbstractData *output ( void );
    
    void emitProgress(int progress);
    
private:
    DTK_DECLARE_PRIVATE(itkMorphologicalFiltersProcessBase)
    
    using dtkAbstractProcess::description;
};

//ITKFILTERSPLUGIN_EXPORT QDebug operator<<(QDebug debug, const itkMorphologicalFiltersProcessBase& process);
//ITKFILTERSPLUGIN_EXPORT QDebug operator<<(QDebug debug,       itkMorphologicalFiltersProcessBase *process);



