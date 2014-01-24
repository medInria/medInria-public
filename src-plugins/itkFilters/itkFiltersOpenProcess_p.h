/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractData.h>
#include <dtkCore/dtkSmartPointer.h>

#include "itkFiltersProcessBase_p.h"

#include <medMetaDataKeys.h>

#include "itkImage.h"
#include "itkCommand.h"
#include <itkGrayscaleMorphologicalOpeningImageFilter.h>
#include "itkBinaryBallStructuringElement.h"

class itkFiltersOpenProcess;

class itkFiltersOpenProcessPrivate : public itkFiltersProcessBasePrivate
{
public:
    itkFiltersOpenProcessPrivate(itkFiltersOpenProcess *q = 0) : itkFiltersProcessBasePrivate(q) {}
    itkFiltersOpenProcessPrivate(const itkFiltersOpenProcessPrivate& other) : itkFiltersProcessBasePrivate(other) {}

    virtual ~itkFiltersOpenProcessPrivate(void) {}
    
    int radius;

    template <class PixelType> void update ( void )
    {        
        typedef itk::Image< PixelType, 3 > ImageType;
        typedef itk::BinaryBallStructuringElement < PixelType, 3> StructuringElementType;
        typedef itk::GrayscaleMorphologicalOpeningImageFilter< ImageType, ImageType, StructuringElementType >  OpenType;
        typename OpenType::Pointer openFilter = OpenType::New();
        
        StructuringElementType ball;
        ball.SetRadius(radius);
        ball.CreateStructuringElement();

        openFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
        openFilter->SetKernel ( ball );
        
        callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) this );
        callback->SetCallback ( itkFiltersProcessBasePrivate::eventCallback );
        
        openFilter->AddObserver ( itk::ProgressEvent(), callback );
        
        openFilter->Update();
        output->setData ( openFilter->GetOutput() );
        
        QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " Open filter (" + QString::number(radius) + ")";
        
        output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    }
};

DTK_IMPLEMENT_PRIVATE(itkFiltersOpenProcess, itkFiltersProcessBase)
