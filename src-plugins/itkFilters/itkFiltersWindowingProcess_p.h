/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <itkCommand.h>
#include <itkFiltersProcessBase_p.h>
#include <itkImage.h>
#include <itkIntensityWindowingImageFilter.h>

#include <medAbstractData.h>
#include <medMetaDataKeys.h>
#include <medUtilities.h>

class itkFiltersWindowingProcess;

class itkFiltersWindowingProcessPrivate : public itkFiltersProcessBasePrivate
{
public:
    itkFiltersWindowingProcessPrivate(itkFiltersWindowingProcess *q = 0) : itkFiltersProcessBasePrivate(q) {}
    itkFiltersWindowingProcessPrivate(const itkFiltersWindowingProcessPrivate& other) : itkFiltersProcessBasePrivate(other) {}

    virtual ~itkFiltersWindowingProcessPrivate(void) {}
    
    double minimumIntensityValue;
    double maximumIntensityValue;
    double minimumOutputIntensityValue;
    double maximumOutputIntensityValue;
    
    template <class PixelType> int update ( void )
    {
        typedef itk::Image< PixelType, 3 > ImageType;
        typedef itk::IntensityWindowingImageFilter< ImageType, ImageType >  WindowingFilterType;
        typename WindowingFilterType::Pointer windowingFilter = WindowingFilterType::New();
    
        windowingFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
        windowingFilter->SetWindowMinimum ( ( PixelType ) minimumIntensityValue );
        windowingFilter->SetWindowMaximum ( ( PixelType ) maximumIntensityValue );
        windowingFilter->SetOutputMinimum ( ( PixelType ) minimumOutputIntensityValue );
        windowingFilter->SetOutputMaximum ( ( PixelType ) maximumOutputIntensityValue );
        
        callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) this );
        callback->SetCallback ( itkFiltersWindowingProcessPrivate::eventCallback );
    
        windowingFilter->AddObserver ( itk::ProgressEvent(), callback );
    
        windowingFilter->Update();

        output->setData ( windowingFilter->GetOutput() );
        
        //Set output description metadata
        QString newSeriesDescription = "windowing " + QString::number(minimumIntensityValue)
                + " " + QString::number(maximumIntensityValue);
        medUtilities::setDerivedMetaData(output, input, newSeriesDescription);

        return DTK_SUCCEED;
    }
};

DTK_IMPLEMENT_PRIVATE(itkFiltersWindowingProcess, itkFiltersProcessBase)


