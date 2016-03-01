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

#include <itkBinaryMorphologicalClosingImageFilter.h>
#include <itkCommand.h>
#include <itkImage.h>
#include <itkBinaryBallStructuringElement.h>
#include <itkMinimumMaximumImageFilter.h>

#include <medMetaDataKeys.h>

class itkFiltersBinaryCloseProcess;

class itkFiltersBinaryCloseProcessPrivate : public itkMorphologicalFiltersProcessBasePrivate
{
public:
    itkFiltersBinaryCloseProcessPrivate(itkFiltersBinaryCloseProcess *q = 0) : itkMorphologicalFiltersProcessBasePrivate(q) {}
    itkFiltersBinaryCloseProcessPrivate(const itkFiltersBinaryCloseProcessPrivate& other) : itkMorphologicalFiltersProcessBasePrivate(other) {}

    virtual ~itkFiltersBinaryCloseProcessPrivate(void) {}

    template <class PixelType> void update ( void )
    {
        typedef itk::Image< PixelType, 3 > ImageType;

        if(!isRadiusInPixels)
        {
            convertMmInPixels<ImageType>();
        }

        typedef itk::FlatStructuringElement < 3> StructuringElementType;
        StructuringElementType::RadiusType elementRadius;
        elementRadius[0] = radius[0]; //radius (double) is truncated
        elementRadius[1] = radius[1];
        elementRadius[2] = radius[2];
        StructuringElementType ball = StructuringElementType::Ball(elementRadius);

        typedef itk::MinimumMaximumImageFilter <ImageType> ImageCalculatorFilterType;
        typename ImageCalculatorFilterType::Pointer imageCalculatorFilter = ImageCalculatorFilterType::New();
        imageCalculatorFilter->SetInput( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
        imageCalculatorFilter->Update();

        typedef itk::BinaryMorphologicalClosingImageFilter< ImageType, ImageType, StructuringElementType >  FilterType;
        typename FilterType::Pointer Filter = FilterType::New();
        Filter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
        Filter->SetKernel ( ball );
        Filter->SetForegroundValue(imageCalculatorFilter->GetMaximum());

        callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) this );
        callback->SetCallback ( itkMorphologicalFiltersProcessBasePrivate::eventCallback );

        Filter->AddObserver ( itk::ProgressEvent(), callback );

        Filter->Update();
        output->setData ( Filter->GetOutput() );

        // Add description on output data
        QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );

        if (isRadiusInPixels)
            newSeriesDescription += " Bin.Close filter\n("+ QString::number(floor(radius[0]))+", "+
            QString::number(floor(radius[1]))+", "+ QString::number(floor(radius[2]))+" pixels)";
        else
            newSeriesDescription += " Bin.Close filter\n("+ QString::number(radiusMm[0])+", "+
            QString::number(radiusMm[1])+", "+ QString::number(radiusMm[2])+" mm)";

        output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    }
};

DTK_IMPLEMENT_PRIVATE(itkFiltersBinaryCloseProcess, itkMorphologicalFiltersProcessBase)
