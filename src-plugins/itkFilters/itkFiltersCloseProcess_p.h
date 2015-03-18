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
#include <itkGrayscaleMorphologicalClosingImageFilter.h>
#include <itkBinaryBallStructuringElement.h>

class itkFiltersCloseProcess;

class itkFiltersCloseProcessPrivate : public itkFiltersProcessBasePrivate
{
public:
    itkFiltersCloseProcessPrivate(itkFiltersCloseProcess *q = 0) : itkFiltersProcessBasePrivate(q) {}
    itkFiltersCloseProcessPrivate(const itkFiltersCloseProcessPrivate& other) : itkFiltersProcessBasePrivate(other) {}

    virtual ~itkFiltersCloseProcessPrivate(void) {}
    
    int radius;

    template <class PixelType> void update ( void )
    {
        typedef itk::Image< PixelType, 3 > ImageType;
        typedef itk::BinaryBallStructuringElement < PixelType, 3> StructuringElementType;
        typedef itk::GrayscaleMorphologicalClosingImageFilter< ImageType, ImageType, StructuringElementType >  CloseType;
        typename CloseType::Pointer closeFilter = CloseType::New();

        StructuringElementType ball;
        ball.SetRadius(radius);
        ball.CreateStructuringElement();

        closeFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
        closeFilter->SetKernel ( ball );
        
        callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) this );
        callback->SetCallback ( itkFiltersProcessBasePrivate::eventCallback );
        
        closeFilter->AddObserver ( itk::ProgressEvent(), callback );
        
        closeFilter->Update();
        output->setData ( closeFilter->GetOutput() );
        
        QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " Close filter (" + QString::number(radius) + ")";
        
        output->copyMetaDataFrom(input);
        output->setMetaData ( medMetaDataKeys::StudyDescription.key(), "mask");
        output->setMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    }
};

DTK_IMPLEMENT_PRIVATE(itkFiltersCloseProcess, itkFiltersProcessBase)
