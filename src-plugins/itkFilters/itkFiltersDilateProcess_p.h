/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#ifndef itkFiltersDilateProcess_P_H
#define itkFiltersDilateProcess_P_H

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkSmartPointer.h>

#include "itkFiltersProcessBase_p.h"

#include <medMetaDataKeys.h>

#include "itkImage.h"
#include "itkCommand.h"
#include "itkGrayscaleDilateImageFilter.h"
#include "itkBinaryBallStructuringElement.h"


class itkFiltersDilateProcess;

class itkFiltersDilateProcessPrivate : public itkFiltersProcessBasePrivate
{
public:
    itkFiltersDilateProcessPrivate(itkFiltersDilateProcess *q = 0) : itkFiltersProcessBasePrivate(q) {}
    itkFiltersDilateProcessPrivate(const itkFiltersDilateProcessPrivate& other) : itkFiltersProcessBasePrivate(other) {}

    virtual ~itkFiltersDilateProcessPrivate(void) {}
    
    int size;

    template <class PixelType> void update ( void )
    {
        typedef itk::Image< PixelType, 3 > ImageType;
        typedef itk::BinaryBallStructuringElement < PixelType, 3> KernelType;
        typedef itk::GrayscaleDilateImageFilter< ImageType, ImageType,KernelType >  DilateType;
        typename DilateType::Pointer dilateFilter = DilateType::New();
        
        KernelType ball;
        typename KernelType::SizeType ballSize;
        ballSize.Fill(size);
        
        ball.SetRadius(ballSize);

        dilateFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
        dilateFilter->SetKernel ( ball );
        
        callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) this );
        callback->SetCallback ( itkFiltersProcessBasePrivate::eventCallback );
        
        dilateFilter->AddObserver ( itk::ProgressEvent(), callback );
        
        dilateFilter->Update();
        output->setData ( dilateFilter->GetOutput() );
        
        QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " Dilate filter (" + QString::number(size) + ")";
        
        output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    }
};

DTK_IMPLEMENT_PRIVATE(itkFiltersDilateProcess, itkFiltersProcessBase)

#endif // itkFiltersDilateProcess_P_H
