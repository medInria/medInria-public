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

#include <itkFiltersProcessBase_p.h>

#include <medMetaDataKeys.h>

#include <itkImage.h>
#include <itkCommand.h>
#include <itkMedianImageFilter.h>

class itkFiltersMedianProcess;

class itkFiltersMedianProcessPrivate : public itkFiltersProcessBasePrivate
{
public:
    itkFiltersMedianProcessPrivate(itkFiltersMedianProcess *q = 0) : itkFiltersProcessBasePrivate(q) {}
    itkFiltersMedianProcessPrivate(const itkFiltersMedianProcessPrivate& other) : itkFiltersProcessBasePrivate(other) {}

    virtual ~itkFiltersMedianProcessPrivate(void) {}
    
    double addValue;
    
    template <class PixelType> int update ( void )
    {
        typedef itk::Image< PixelType, 3 > ImageType;
        typedef itk::MedianImageFilter< ImageType, ImageType >  MedianFilterType;
        typename MedianFilterType::Pointer medianFilter = MedianFilterType::New();
    
        medianFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
        
        callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) this );
        callback->SetCallback ( itkFiltersMedianProcessPrivate::eventCallback );
    
        medianFilter->AddObserver ( itk::ProgressEvent(), callback );
    
        try
        {
            medianFilter->Update();
        }
        catch( itk::ExceptionObject & err )
        {
            qDebug() << "ExceptionObject caught in itkFiltersMedianProcess! " << err.GetDescription();
            return DTK_FAILURE;
        }

        output->setData ( medianFilter->GetOutput() );
        
        //Set output description metadata
        QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " median filter";
        
        output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );

        return DTK_SUCCEED;
    }
};

DTK_IMPLEMENT_PRIVATE(itkFiltersMedianProcess, itkFiltersProcessBase)


