/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkSmartPointer.h>

#include "itkFiltersProcessBase_p.h"

#include <medMetaDataKeys.h>

#include "itkImage.h"
#include "itkCommand.h"
#include <itkConnectedComponentImageFilter.h>
#include <itkRelabelComponentImageFilter.h>
#include "itkBinaryThresholdImageFilter.h"

class itkFiltersComponentSizeThresholdProcess;

class itkFiltersComponentSizeThresholdProcessPrivate : public itkFiltersProcessBasePrivate
{
public:
    itkFiltersComponentSizeThresholdProcessPrivate(itkFiltersComponentSizeThresholdProcess *q = 0) : itkFiltersProcessBasePrivate(q) {}
    itkFiltersComponentSizeThresholdProcessPrivate(const itkFiltersComponentSizeThresholdProcessPrivate& other) : itkFiltersProcessBasePrivate(other) {}

    virtual ~itkFiltersComponentSizeThresholdProcessPrivate(void) {}
    
    double minimumSize;

    template <class PixelType> void castToUInt3 ( void )
    {
        try
        {
            //we will later label the image so we don't care about precision.
            typedef itk::Image< PixelType, 3 > InputImageType;
            typedef itk::Image< unsigned int, 3 > OutputImageType;
            typedef itk::CastImageFilter< InputImageType, OutputImageType > CastFilterType;

            typename CastFilterType::Pointer  caster = CastFilterType::New();
            typename InputImageType::Pointer im = dynamic_cast< InputImageType*>((itk::Object*)(input->data()));
            caster->SetInput(im);
            caster->Update();
            input = medAbstractDataFactory::instance()->createSmartPointer ( "itkDataImageUInt3" );
            output = medAbstractDataFactory::instance()->createSmartPointer ( "itkDataImageUInt3" );
            input->setData(caster->GetOutput());
        }
        catch( itk::ExceptionObject & err )
        {
            std::cerr << "ExceptionObject caught in sizeThresholdingProcess!" << std::endl;
            std::cerr << err << std::endl;
        }

    }
    
    template <class PixelType> void update ( void )
    {
        try
        {
            typedef itk::Image< PixelType, 3 > ImageType;
            typedef itk::ConnectedComponentImageFilter <ImageType, ImageType> ConnectedComponentFilterType;
            typename ConnectedComponentFilterType::Pointer connectedComponentFilter = ConnectedComponentFilterType::New();
        
            connectedComponentFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
            connectedComponentFilter->Update();

            // RELABEL COMPONENTS according to their sizes (0:largest(background))
            typedef itk::RelabelComponentImageFilter<ImageType, ImageType> FilterType;
            typename FilterType::Pointer relabelFilter = FilterType::New();
            relabelFilter->SetInput(connectedComponentFilter->GetOutput());
            relabelFilter->SetMinimumObjectSize(minimumSize);
            relabelFilter->Update();

            // BINARY FILTER
            typedef itk::BinaryThresholdImageFilter <ImageType, ImageType>
            BinaryThresholdImageFilterType;
 
            typename BinaryThresholdImageFilterType::Pointer thresholdFilter
            = BinaryThresholdImageFilterType::New();
            thresholdFilter->SetInput(relabelFilter->GetOutput());
            thresholdFilter->SetUpperThreshold(0);
            thresholdFilter->SetInsideValue(0);
            thresholdFilter->SetOutsideValue(1);
            thresholdFilter->Update();

        
            callback = itk::CStyleCommand::New();
            callback->SetClientData ( ( void * ) this );
            callback->SetCallback ( itkFiltersProcessBasePrivate::eventCallback );
        
            connectedComponentFilter->AddObserver ( itk::ProgressEvent(), callback );
        
            output->setData ( thresholdFilter->GetOutput() );
        }
        catch( itk::ExceptionObject & err )
        {
            std::cerr << "ExceptionObject caught in sizeThresholdingProcess!" << std::endl;
            std::cerr << err << std::endl;
        }
        
        QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " ConnectedComponent filter (" + QString::number(minimumSize) + ")";

        output->copyMetaDataFrom(input);
        output->setMetaData ( medMetaDataKeys::StudyDescription.key(), "mask");
        output->setMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    }
};

DTK_IMPLEMENT_PRIVATE(itkFiltersComponentSizeThresholdProcess, itkFiltersProcessBase)


