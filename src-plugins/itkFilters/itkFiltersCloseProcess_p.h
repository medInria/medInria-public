/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#ifndef ITKFILTERSCLOSEPROCESS_P_H
#define ITKFILTERSCLOSEPROCESS_P_H

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkSmartPointer.h>

#include "itkFiltersProcessBase_p.h"

#include <medMetaDataKeys.h>

#include "itkImage.h"
#include "itkCommand.h"
#include <itkBinaryMorphologicalClosingImageFilter.h>
#include "itkBinaryBallStructuringElement.h"
#include "itkMinimumMaximumImageCalculator.h"

class itkFiltersCloseProcess;

class itkFiltersCloseProcessPrivate : public itkFiltersProcessBasePrivate
{
public:
    itkFiltersCloseProcessPrivate(itkFiltersCloseProcess *q = 0) : itkFiltersProcessBasePrivate(q) {}
    itkFiltersCloseProcessPrivate(const itkFiltersCloseProcessPrivate& other) : itkFiltersProcessBasePrivate(other) {}

    virtual ~itkFiltersCloseProcessPrivate(void) {}
    
    int size;

    template <class PixelType> void update ( void )
    {
        typedef itk::Image< PixelType, 3 > ImageType;
        typedef itk::BinaryBallStructuringElement < PixelType, 3> KernelType;
        typedef itk::BinaryMorphologicalClosingImageFilter< ImageType, ImageType, KernelType >  CloseType;
        typename CloseType::Pointer closeFilter = CloseType::New();

        KernelType ball;
        typename KernelType::SizeType ballSize;
        ballSize.Fill(size);
        
        ball.SetRadius(ballSize);

        closeFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
        closeFilter->SetKernel ( ball );

        typedef itk::MinimumMaximumImageCalculator <ImageType> ImageCalculatorFilterType;
        ImageCalculatorFilterType::Pointer imageCalculatorFilter = ImageCalculatorFilterType::New ();
        imageCalculatorFilter->SetImage(dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ));
        imageCalculatorFilter->ComputeMaximum();
        closeFilter->SetForegroundValue(imageCalculatorFilter->GetMaximum());
        
        callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) this );
        callback->SetCallback ( itkFiltersProcessBasePrivate::eventCallback );
        
        closeFilter->AddObserver ( itk::ProgressEvent(), callback );
        
        closeFilter->Update();
        output->setData ( closeFilter->GetOutput() );
        
        QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " Close filter (" + QString::number(size) + ")";
        
        output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    }
};

DTK_IMPLEMENT_PRIVATE(itkFiltersCloseProcess, itkFiltersProcessBase)

#endif // ITKFILTERSCLOSEPROCESS_P_H
