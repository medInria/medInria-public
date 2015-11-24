/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <itkMorphologicalFiltersProcessBase_p.h>

#include <itkImage.h>
#include <itkCommand.h>
#include <itkBinaryMorphologicalOpeningImageFilter.h>
#include <itkMinimumMaximumImageFilter.h>
#include <medMetaDataKeys.h>

class itkFiltersBinaryOpenProcess;

class itkFiltersBinaryOpenProcessPrivate : public itkMorphologicalFiltersProcessBasePrivate
{
public:
    itkFiltersBinaryOpenProcessPrivate(itkFiltersBinaryOpenProcess *q = 0) : itkMorphologicalFiltersProcessBasePrivate(q) {}
    itkFiltersBinaryOpenProcessPrivate(const itkFiltersBinaryOpenProcessPrivate& other) : itkMorphologicalFiltersProcessBasePrivate(other) {}

    virtual ~itkFiltersBinaryOpenProcessPrivate(void) {}

    template <class PixelType> void update ( void )
    {
        typedef itk::Image< PixelType, 3 > ImageType;

        if(!isRadiusInPixels)
        {
            convertMmInPixels<ImageType>();
        }

        typedef itk::FlatStructuringElement < 3> StructuringElementType;
        StructuringElementType::RadiusType elementRadius;
        elementRadius[0] = radius[0];
        elementRadius[1] = radius[1];
        elementRadius[2] = radius[2];
        StructuringElementType ball = StructuringElementType::Ball(elementRadius);

        typedef itk::MinimumMaximumImageFilter <ImageType> ImageCalculatorFilterType;
        typename ImageCalculatorFilterType::Pointer imageCalculatorFilter = ImageCalculatorFilterType::New();
        imageCalculatorFilter->SetInput( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
        imageCalculatorFilter->Update();

        typedef itk::BinaryMorphologicalOpeningImageFilter< ImageType, ImageType, StructuringElementType >  FilterType;
        typename FilterType::Pointer Filter = FilterType::New();
        Filter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
        Filter->SetKernel ( ball );
        Filter->SetBackgroundValue(imageCalculatorFilter->GetMinimum());
        Filter->SetForegroundValue(imageCalculatorFilter->GetMaximum());

        callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) this );
        callback->SetCallback ( itkMorphologicalFiltersProcessBasePrivate::eventCallback );

        Filter->AddObserver ( itk::ProgressEvent(), callback );

        Filter->Update();
        output->setData ( Filter->GetOutput() );

        // Add description on output data
        QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );

        newSeriesDescription += " Bin.Open filter\n("+ QString::number(radius[0])+", "+
                QString::number(radius[1])+", "+ QString::number(radius[2]);
        isRadiusInPixels? newSeriesDescription += " px": newSeriesDescription += " mm";

        output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    }
};

DTK_IMPLEMENT_PRIVATE(itkFiltersBinaryOpenProcess, itkMorphologicalFiltersProcessBase)
