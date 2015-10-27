/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCoreSupport/dtkAbstractProcess.h>
#include <dtkCoreSupport/dtkAbstractProcess_p.h>
#include <medAbstractImageData.h>
#include <dtkLog/dtkLog.h>

#include <itkFiltersPluginExport.h>

#include <itkProcessObject.h>
#include <itkCommand.h>

class itkFiltersProcessBase;

class ITKFILTERSPLUGIN_EXPORT itkFiltersProcessBasePrivate : public dtkAbstractProcessPrivate
{
public:
    itkFiltersProcessBasePrivate(itkFiltersProcessBase *q = 0) : dtkAbstractProcessPrivate(q) {}
    itkFiltersProcessBasePrivate(const itkFiltersProcessBasePrivate& other) : dtkAbstractProcessPrivate(other) {}

    virtual ~itkFiltersProcessBasePrivate(void) {}
    
    QString description;
    QString filterType;
    
    itk::CStyleCommand::Pointer callback;
    itkFiltersProcessBase *filter;
    
    dtkSmartPointer <medAbstractImageData> input;
    dtkSmartPointer <medAbstractImageData> output;
    
    template <class PixelType> void setupFilter() {}
    virtual void setFilterDescription() {}
    
    static void eventCallback ( itk::Object *caller, const itk::EventObject& event, void *clientData)
    {
        itkFiltersProcessBasePrivate * source = reinterpret_cast<itkFiltersProcessBasePrivate *> ( clientData );
        itk::ProcessObject * processObject = ( itk::ProcessObject* ) caller;
    
        if ( !source ) { dtkWarn() << "Source is null"; }
    
        source->filter->emitProgress((int) (processObject->GetProgress() * 100));
        
    }
};

DTK_IMPLEMENT_PRIVATE(itkFiltersProcessBase, dtkAbstractProcess)


