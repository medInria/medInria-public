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

#include <itkGrayscaleMorphologicalOpeningImageFilter.h>
#include <itkCommand.h>
#include <itkImage.h>
#include <itkMinimumMaximumImageFilter.h>

#include <medMetaDataKeys.h>

class itkFiltersOpenProcess;

class itkFiltersOpenProcessPrivate : public itkMorphologicalFiltersProcessBasePrivate
{
public:
    itkFiltersOpenProcessPrivate(itkFiltersOpenProcess *q = 0) : itkMorphologicalFiltersProcessBasePrivate(q) {}
    itkFiltersOpenProcessPrivate(const itkFiltersOpenProcessPrivate& other) : itkMorphologicalFiltersProcessBasePrivate(other) {}

    virtual ~itkFiltersOpenProcessPrivate(void) {}

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

        typedef itk::GrayscaleMorphologicalOpeningImageFilter< ImageType, ImageType, StructuringElementType >  OpenType;
        typename OpenType::Pointer openFilter = OpenType::New();
        openFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
        openFilter->SetKernel ( ball );
        
        callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) this );
        callback->SetCallback ( itkMorphologicalFiltersProcessBasePrivate::eventCallback );
        
        openFilter->AddObserver ( itk::ProgressEvent(), callback );
        
        openFilter->Update();
        output->setData ( openFilter->GetOutput() );
        
        QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );

        // Add description on output data
        if (isRadiusInPixels)
            newSeriesDescription += " Open filter\n("+ QString::number(radius[0])+", "+
            QString::number(radius[1])+", "+ QString::number(radius[2])+" pixels)";
        else
            newSeriesDescription += " Open filter\n("+ QString::number(radiusMm[0])+", "+
            QString::number(radiusMm[1])+", "+ QString::number(radiusMm[2])+" mm)";
        
        output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    }
};

DTK_IMPLEMENT_PRIVATE(itkFiltersOpenProcess, itkMorphologicalFiltersProcessBase)
