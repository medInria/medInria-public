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
#include <itkInvertIntensityImageFilter.h>
#include <itkMinimumMaximumImageCalculator.h>

class itkFiltersInvertProcess;

class itkFiltersInvertProcessPrivate : public itkFiltersProcessBasePrivate
{
public:
    itkFiltersInvertProcessPrivate(itkFiltersInvertProcess *q = 0) : itkFiltersProcessBasePrivate(q) {}
    itkFiltersInvertProcessPrivate(const itkFiltersInvertProcessPrivate& other) : itkFiltersProcessBasePrivate(other) {}

    virtual ~itkFiltersInvertProcessPrivate(void) {}
    
    double addValue;
    
    template <class PixelType> int update ( void )
    {
        typedef itk::Image< PixelType, 3 > ImageType;
        typedef itk::InvertIntensityImageFilter< ImageType, ImageType >  InvertFilterType;
        typename InvertFilterType::Pointer invertFilter = InvertFilterType::New();

        // compute maximum intensity of image
        typedef itk::MinimumMaximumImageCalculator< ImageType > MinMaxFilterType;
        typename MinMaxFilterType::Pointer maxFilter = MinMaxFilterType::New();

        maxFilter->SetImage( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
        maxFilter->Compute();
        PixelType maximum = maxFilter->GetMaximum();
        PixelType minimum = maxFilter->GetMinimum();

        invertFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
        invertFilter->SetMaximum(maximum + minimum);

    
        invertFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
        
        callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) this );
        callback->SetCallback ( itkFiltersInvertProcessPrivate::eventCallback );
    
        invertFilter->AddObserver ( itk::ProgressEvent(), callback );

        invertFilter->Update();

        output->setData ( invertFilter->GetOutput() );
        
        //Set output description metadata
        QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " invert filter";
    
        output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );

        return DTK_SUCCEED;
    }
};

DTK_IMPLEMENT_PRIVATE(itkFiltersInvertProcess, itkFiltersProcessBase)


