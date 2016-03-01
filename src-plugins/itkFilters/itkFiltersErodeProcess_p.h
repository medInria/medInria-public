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
#include <itkGrayscaleErodeImageFilter.h>
#include <itkBinaryBallStructuringElement.h>

class itkFiltersErodeProcess;

class itkFiltersErodeProcessPrivate : public itkMorphologicalFiltersProcessBasePrivate
{
public:
    itkFiltersErodeProcessPrivate(itkFiltersErodeProcess *q = 0) : itkMorphologicalFiltersProcessBasePrivate(q) {}
    itkFiltersErodeProcessPrivate(const itkFiltersErodeProcessPrivate& other) : itkMorphologicalFiltersProcessBasePrivate(other) {}

    virtual ~itkFiltersErodeProcessPrivate(void) {}

    template <class PixelType> void update ( void )
    {
        typedef itk::Image< PixelType, 3 > ImageType;
        
        if(!isRadiusInPixels)
            convertMmInPixels<ImageType>();

        typedef itk::FlatStructuringElement < 3> StructuringElementType;
        StructuringElementType::RadiusType elementRadius;
        elementRadius[0] = radius[0];  //radius (double) is truncated
        elementRadius[1] = radius[1];
        elementRadius[2] = radius[2];
        
        typedef itk::GrayscaleErodeImageFilter< ImageType, ImageType,StructuringElementType >  ErodeType;
        typename ErodeType::Pointer erodeFilter = ErodeType::New();
        
        StructuringElementType ball = StructuringElementType::Ball(elementRadius);

        erodeFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
        erodeFilter->SetKernel ( ball );
        
        callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) this );
        callback->SetCallback ( itkMorphologicalFiltersProcessBasePrivate::eventCallback );
        
        erodeFilter->AddObserver ( itk::ProgressEvent(), callback );
        erodeFilter->Update();
        output->setData ( erodeFilter->GetOutput() );
        
        QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );

        if (isRadiusInPixels)
            newSeriesDescription += " Erode filter\n("+ QString::number(floor(radius[0]))+", "+
            QString::number(floor(radius[1]))+", "+ QString::number(floor(radius[2]))+" pixels)";
        else
            newSeriesDescription += " Erode filter\n("+ QString::number(radiusMm[0])+", "+ 
            QString::number(radiusMm[1])+", "+ QString::number(radiusMm[2])+" mm)";
        
        output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    }
};

DTK_IMPLEMENT_PRIVATE(itkFiltersErodeProcess, itkMorphologicalFiltersProcessBase)
