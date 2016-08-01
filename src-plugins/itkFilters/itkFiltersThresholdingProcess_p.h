/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkSmartPointer.h>

#include <itkCommand.h>
#include <itkFiltersProcessBase_p.h>
#include <itkImage.h>
#include <itkThresholdImageFilter.h>

#include <medAbstractData.h>
#include <medMetaDataKeys.h>
#include <medUtilities.h>

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
    
    template <class PixelType> int update ( void )
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
    
        try
        {
            thresholdFilter->Update();
        }
        catch( itk::ExceptionObject & err )
        {
            qDebug() << "ExceptionObject caught in itkFiltersThresholdingProcess! " << err.GetDescription();
            return DTK_FAILURE;
        }

        output->setData ( thresholdFilter->GetOutput() );

        QString newSeriesDescription = "threshold " + QString::number(threshold);
        medUtilities::setDerivedMetaData(output, input, newSeriesDescription);

        return DTK_SUCCEED;
    }
};

DTK_IMPLEMENT_PRIVATE(itkFiltersThresholdingProcess, itkFiltersProcessBase)


