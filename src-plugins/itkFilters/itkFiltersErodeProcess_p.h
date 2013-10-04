/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#ifndef ITKFILTERSERODEPROCESS_P_H
#define ITKFILTERSERODEPROCESS_P_H

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkSmartPointer.h>

#include "itkFiltersProcessBase_p.h"

#include <medMetaDataKeys.h>

#include "itkImage.h"
#include "itkCommand.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkBinaryBallStructuringElement.h"

class itkFiltersErodeProcess;

class itkFiltersErodeProcessPrivate : public itkFiltersProcessBasePrivate
{
public:
    itkFiltersErodeProcessPrivate(itkFiltersErodeProcess *q = 0) : itkFiltersProcessBasePrivate(q) {}
    itkFiltersErodeProcessPrivate(const itkFiltersErodeProcessPrivate& other) : itkFiltersProcessBasePrivate(other) {}

    virtual ~itkFiltersErodeProcessPrivate(void) {}
    
    int size;

    template <class PixelType> void update ( void )
    {        
        typedef itk::Image< PixelType, 3 > ImageType;
        typedef itk::BinaryBallStructuringElement < PixelType, 3> KernelType;
        typedef itk::BinaryErodeImageFilter< ImageType, ImageType,KernelType >  ErodeType;
        typename ErodeType::Pointer erodeFilter = ErodeType::New();
        
        KernelType ball;
        typename KernelType::SizeType ballSize;
        ballSize.Fill(size);
        
        ball.SetRadius(ballSize);

        erodeFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
        erodeFilter->SetKernel ( ball );
        erodeFilter->SetErodeValue(1);
        
        callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) this );
        callback->SetCallback ( itkFiltersProcessBasePrivate::eventCallback );
        
        erodeFilter->AddObserver ( itk::ProgressEvent(), callback );
        
        erodeFilter->Update();
        output->setData ( erodeFilter->GetOutput() );
        
        QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " Erode filter (" + QString::number(size) + ")";
        
        output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    }
};

DTK_IMPLEMENT_PRIVATE(itkFiltersErodeProcess, itkFiltersProcessBase)

#endif // ITKFILTERSERODEPROCESS_P_H
