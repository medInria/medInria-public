/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCoreSupport/dtkSmartPointer.h>

#include <itkCommand.h>
#include <itkFiltersPluginExport.h>

#include <medAbstractImageData.h>
#include <medAbstractProcessLegacy.h>

class itkFiltersProcessBasePrivate;

class ITKFILTERSPLUGIN_EXPORT itkFiltersProcessBase : public medAbstractProcessLegacy
{
    Q_OBJECT

public:
    itkFiltersProcessBase(itkFiltersProcessBase *parent = nullptr);
    itkFiltersProcessBase(const itkFiltersProcessBase& other);
    itkFiltersProcessBase& operator = (const itkFiltersProcessBase& other);
    
    void setInput(medAbstractData *data, int channel = 0);

    medAbstractData *output();
    
    void emitProgress(int progress);

    virtual int tryUpdate()
    {
        return medAbstractProcessLegacy::FAILURE;
    }
    int update();

    static void eventCallback ( itk::Object *caller, const itk::EventObject& event, void *clientData);

protected:
    dtkSmartPointer<medAbstractImageData> getInputData();
    void setInputData(dtkSmartPointer<medAbstractImageData> inputData);
    dtkSmartPointer<medAbstractImageData> getOutputData();
    void setOutputData(dtkSmartPointer<medAbstractImageData> outputData);

private:
    itkFiltersProcessBasePrivate *d;
};
