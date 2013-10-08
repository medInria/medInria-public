/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#ifndef ITKFILTERSOPENPROCESS_P_H
#define ITKFILTERSOPENPROCESS_P_H

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkSmartPointer.h>

#include "itkFiltersProcessBase_p.h"

#include <medMetaDataKeys.h>

#include "itkImage.h"
#include "itkCommand.h"
#include <itkBinaryMorphologicalOpeningImageFilter.h>
#include "itkBinaryBallStructuringElement.h"
#include "itkMinimumMaximumImageCalculator.h"

class itkFiltersOpenProcess;

class itkFiltersOpenProcessPrivate : public itkFiltersProcessBasePrivate
{
public:
    itkFiltersOpenProcessPrivate(itkFiltersOpenProcess *q = 0) : itkFiltersProcessBasePrivate(q) {}
    itkFiltersOpenProcessPrivate(const itkFiltersOpenProcessPrivate& other) : itkFiltersProcessBasePrivate(other) {}

    virtual ~itkFiltersOpenProcessPrivate(void) {}
    
    int size;

    template <class PixelType> void update ( void )
    {        
        typedef itk::Image< PixelType, 3 > ImageType;
        typedef itk::BinaryBallStructuringElement < PixelType, 3> KernelType;
        typedef itk::BinaryMorphologicalOpeningImageFilter< ImageType, ImageType, KernelType >  OpenType;
        typename OpenType::Pointer openFilter = OpenType::New();
        
        KernelType ball;
        typename KernelType::SizeType ballSize;
        ballSize.Fill(size);
        
        ball.SetRadius(ballSize);

        openFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
        openFilter->SetKernel ( ball );

        typedef itk::MinimumMaximumImageCalculator <ImageType> ImageCalculatorFilterType;
        ImageCalculatorFilterType::Pointer imageCalculatorFilter = ImageCalculatorFilterType::New ();
        imageCalculatorFilter->SetImage(dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ));
        imageCalculatorFilter->ComputeMaximum();
        openFilter->SetForegroundValue(imageCalculatorFilter->GetMaximum());
        
        callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) this );
        callback->SetCallback ( itkFiltersProcessBasePrivate::eventCallback );
        
        openFilter->AddObserver ( itk::ProgressEvent(), callback );
        
        openFilter->Update();
        output->setData ( openFilter->GetOutput() );
        
        QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " Open filter (" + QString::number(size) + ")";
        
        output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    }
};

DTK_IMPLEMENT_PRIVATE(itkFiltersOpenProcess, itkFiltersProcessBase)

#endif // ITKFILTERSOPENPROCESS_P_H
