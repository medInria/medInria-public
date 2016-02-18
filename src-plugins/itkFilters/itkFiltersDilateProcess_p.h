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

#include <itkMorphologicalFiltersProcessBase_p.h>

#include <medMetaDataKeys.h>

#include <itkImage.h>
#include <itkCommand.h>
#include <itkBinaryDilateImageFilter.h>
#include <itkBinaryBallStructuringElement.h>
#include <itkMinimumMaximumImageCalculator.h>


class itkFiltersDilateProcess;

class itkFiltersDilateProcessPrivate : public itkMorphologicalFiltersProcessBasePrivate
{
public:
    itkFiltersDilateProcessPrivate(itkFiltersDilateProcess *q = 0) : itkMorphologicalFiltersProcessBasePrivate(q) {}
    itkFiltersDilateProcessPrivate(const itkFiltersDilateProcessPrivate& other) : itkMorphologicalFiltersProcessBasePrivate(other) {}

    virtual ~itkFiltersDilateProcessPrivate(void) {}

    template <class PixelType> void update ( void )
    {
        typedef itk::Image< PixelType, 3 > ImageType;

        if(!isRadiusInPixels)
            convertMmInPixels<ImageType>();

        typedef itk::FlatStructuringElement < 3> StructuringElementType;
        StructuringElementType::RadiusType elementRadius;
        elementRadius[0] = radius[0];
        elementRadius[1] = radius[1];
        elementRadius[2] = radius[2];

        typedef itk::BinaryDilateImageFilter< ImageType, ImageType,StructuringElementType >  DilateType;
        typename DilateType::Pointer dilateFilter = DilateType::New();

        StructuringElementType ball = StructuringElementType::Ball(elementRadius);

        dilateFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
        dilateFilter->SetKernel ( ball );

        typedef itk::MinimumMaximumImageCalculator <ImageType> ImageCalculatorFilterType;
        typename ImageCalculatorFilterType::Pointer imageCalculatorFilter
                = ImageCalculatorFilterType::New ();
        imageCalculatorFilter->SetImage(dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ));
        imageCalculatorFilter->ComputeMaximum();

        dilateFilter->SetDilateValue(imageCalculatorFilter->GetMaximum());

        callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) this );
        callback->SetCallback ( itkMorphologicalFiltersProcessBasePrivate::eventCallback );

        dilateFilter->AddObserver ( itk::ProgressEvent(), callback );
        dilateFilter->Update();
        output->setData ( dilateFilter->GetOutput() );

        QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );

        if (isRadiusInPixels)
            newSeriesDescription += " Dilate filter\n("+ QString::number(radius[0])+", "+ 
            QString::number(radius[1])+", "+ QString::number(radius[2])+" pixels)";
        else
            newSeriesDescription += " Dilate filter\n("+ QString::number(radiusMm[0])+", "+ 
            QString::number(radiusMm[1])+", "+ QString::number(radiusMm[2])+" mm)";

        output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    }
};

DTK_IMPLEMENT_PRIVATE(itkFiltersDilateProcess, itkMorphologicalFiltersProcessBase)
