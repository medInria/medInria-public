/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkSmartPointer.h>

#include <itkCommand.h>
#include <itkFiltersPluginExport.h>

#include <medAbstractImageData.h>
#include <medAbstractProcess.h>

class ITKFILTERSPLUGIN_EXPORT itkFiltersProcessBase : public medAbstractProcess
{
    Q_OBJECT

public:
    itkFiltersProcessBase(itkFiltersProcessBase * parent = 0);
    
    itkFiltersProcessBase& operator = (const itkFiltersProcessBase& other);

    QString description ( void );
    
    void setInput ( medAbstractData *data, int channel = 0 );

    medAbstractData *output ( void );
    
    void emitProgress(int progress);

    virtual int tryUpdate(){return DTK_FAILURE;}
    int update();

    static void eventCallback ( itk::Object *caller, const itk::EventObject& event, void *clientData);

protected:
    QString descriptionText;
    itkFiltersProcessBase *filter;
    dtkSmartPointer <medAbstractImageData> inputData;
    dtkSmartPointer <medAbstractImageData> outputData;
    itk::CStyleCommand::Pointer callback;
};


