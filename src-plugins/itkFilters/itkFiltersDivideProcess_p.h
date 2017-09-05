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
#include <itkDivideImageFilter.h>

class itkFiltersDivideProcess;

class itkFiltersDivideProcessPrivate : public itkFiltersProcessBasePrivate
{
public:
    itkFiltersDivideProcessPrivate(itkFiltersDivideProcess *q = 0) : itkFiltersProcessBasePrivate(q) {}
    itkFiltersDivideProcessPrivate(const itkFiltersDivideProcessPrivate& other) : itkFiltersProcessBasePrivate(other) {}

    virtual ~itkFiltersDivideProcessPrivate(void) {}
    
    double divideFactor;
    
    template <class PixelType> int update ( void )
    {
        typedef itk::Image< PixelType, 3 > ImageType;
        typedef itk::DivideImageFilter< ImageType, itk::Image<double, ImageType::ImageDimension>, ImageType >  DivideFilterType;
        typename DivideFilterType::Pointer divideFilter = DivideFilterType::New();
    
        divideFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
        divideFilter->SetConstant ( divideFactor );
        
        callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) this );
        callback->SetCallback ( itkFiltersDivideProcessPrivate::eventCallback );
    
        divideFilter->AddObserver ( itk::ProgressEvent(), callback );

        divideFilter->Update();

        output->setData ( divideFilter->GetOutput() );
        
        QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " divide filter (" + QString::number(divideFactor) + ")";
    
        output->setMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );

        return DTK_SUCCEED;
    }
};

DTK_IMPLEMENT_PRIVATE(itkFiltersDivideProcess, itkFiltersProcessBase)



