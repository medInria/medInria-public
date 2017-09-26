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

    static const double initAddValue       = 100.0;
    static const double initSubtractValue  = 100.0;
    static const double initMultiplyFactor = 2.0;
    static const double initDivideFactor   = 2.0;
    static const double initSigma          = 1.0;
    static const double initThreshold          = 200.0;
    static const int    initOutsideValue       = 0;
    static const bool   initComparisonOperator = true;
    static const double initMinimumSize        = 50.0;
    static const int    initShrinkFactors(int index)
    {
        static const int a[] = {1,1,1};
        return a[index];
    }

    itkFiltersProcessBase(itkFiltersProcessBase * parent = 0);
    itkFiltersProcessBase(const itkFiltersProcessBase& other);
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
    dtkSmartPointer <medAbstractImageData> inputData;
    dtkSmartPointer <medAbstractImageData> outputData;
    itk::CStyleCommand::Pointer callback;
};


