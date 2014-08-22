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
#include "itkThresholdImageFilter.h"

class itkFiltersThresholdingProcess;

class itkFiltersThresholdingProcessPrivate : public itkFiltersProcessBasePrivate
{
public:
    itkFiltersThresholdingProcessPrivate(itkFiltersThresholdingProcess *q = 0) : itkFiltersProcessBasePrivate(q) {}
    itkFiltersThresholdingProcessPrivate(const itkFiltersThresholdingProcessPrivate& other) : itkFiltersProcessBasePrivate(other) {}

    virtual ~itkFiltersThresholdingProcessPrivate(void) {}
    
    double threshold;
    int outsideValue;
    bool comparisonOperator;
    
    template <class PixelType> void update ( void )
    {
        typedef itk::Image< PixelType, 3 > ImageType;
        typedef itk::ThresholdImageFilter < ImageType>  ThresholdImageFilterType;
        typename ThresholdImageFilterType::Pointer thresholdFilter = ThresholdImageFilterType::New();
    
        thresholdFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
        if (comparisonOperator)
            thresholdFilter->SetUpper( threshold ); // <= threshold
        else
            thresholdFilter->SetLower( threshold );
        thresholdFilter->SetOutsideValue( outsideValue );
        
        callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) this );
        callback->SetCallback ( itkFiltersThresholdingProcessPrivate::eventCallback );
    
        thresholdFilter->AddObserver ( itk::ProgressEvent(), callback );
    
        thresholdFilter->Update();
        output->setData ( thresholdFilter->GetOutput() );
        
        QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " threshold (" + QString::number(threshold) + ")";
    
        output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    }
};

DTK_IMPLEMENT_PRIVATE(itkFiltersThresholdingProcess, itkFiltersProcessBase)


