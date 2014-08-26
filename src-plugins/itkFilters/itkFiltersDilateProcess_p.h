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
#include <itkGrayscaleDilateImageFilter.h>
#include <itkBinaryBallStructuringElement.h>


class itkFiltersDilateProcess;

class itkFiltersDilateProcessPrivate : public itkFiltersProcessBasePrivate
{
public:
    itkFiltersDilateProcessPrivate(itkFiltersDilateProcess *q = 0) : itkFiltersProcessBasePrivate(q) {}
    itkFiltersDilateProcessPrivate(const itkFiltersDilateProcessPrivate& other) : itkFiltersProcessBasePrivate(other) {}

    virtual ~itkFiltersDilateProcessPrivate(void) {}
        
    int radius[3];
    double radiusMm[3];
    bool isRadiusInPixels;
    int radiusInPixels;

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

        typedef itk::GrayscaleDilateImageFilter< ImageType, ImageType,StructuringElementType >  DilateType;
        typename DilateType::Pointer dilateFilter = DilateType::New();

        StructuringElementType ball = StructuringElementType::Ball(elementRadius);

        dilateFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
        dilateFilter->SetKernel ( ball );

        callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) this );
        callback->SetCallback ( itkFiltersProcessBasePrivate::eventCallback );

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
    
    template <class ImageType> void convertMmInPixels ( void )
    {
        ImageType *image = dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) );
        for (int i=0; i<image->GetSpacing().Size(); i++)
        {
            radius[i] = floor((radius[i]/image->GetSpacing()[i])+0.5); //rounding
            radiusMm[i] = radius[i] * image->GetSpacing()[i];
        }
    }
};

DTK_IMPLEMENT_PRIVATE(itkFiltersDilateProcess, itkFiltersProcessBase)
