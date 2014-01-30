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

#include <itkFiltersProcessBase_p.h>

#include <medMetaDataKeys.h>

#include <itkImage.h>
#include <itkCommand.h>
#include <itkSmoothingRecursiveGaussianImageFilter.h>

class itkFiltersGaussianProcess;

class itkFiltersGaussianProcessPrivate : public itkFiltersProcessBasePrivate
{
public:
    itkFiltersGaussianProcessPrivate(itkFiltersGaussianProcess *q = 0) : itkFiltersProcessBasePrivate(q) {}
    itkFiltersGaussianProcessPrivate(const itkFiltersGaussianProcessPrivate& other) : itkFiltersProcessBasePrivate(other) {}

    virtual ~itkFiltersGaussianProcessPrivate(void) {}
    
    double sigma;
    
    template <class PixelType> void update ( void )
    {
        typedef itk::Image< PixelType, 3 > ImageType;
        typedef itk::SmoothingRecursiveGaussianImageFilter< ImageType, ImageType >  GaussianFilterType;
        typename GaussianFilterType::Pointer gaussianFilter = GaussianFilterType::New();
    
        gaussianFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
        gaussianFilter->SetSigma( sigma );
        
        callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) this );
        callback->SetCallback ( itkFiltersGaussianProcessPrivate::eventCallback );
    
        gaussianFilter->AddObserver ( itk::ProgressEvent(), callback );
    
        gaussianFilter->Update();
        output->setData ( gaussianFilter->GetOutput() );
        
        QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " gaussian filter (" + QString::number(sigma) + ")";
    
        output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    }
};

DTK_IMPLEMENT_PRIVATE(itkFiltersGaussianProcess, itkFiltersProcessBase)


