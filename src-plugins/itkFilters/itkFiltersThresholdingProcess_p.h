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
    
    template <class PixelType> void update ( void )
    {
        try
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
        }
        catch (itk::ExceptionObject & err){
            std::cerr << "ExceptionObject caught !" << std::endl;
            std::cerr << err << std::endl;
        }

        QString newSeriesDescription = "threshold " + QString::number(threshold);
        medUtilities::setDerivedMetaData(output, input, newSeriesDescription);
    }
};

DTK_IMPLEMENT_PRIVATE(itkFiltersThresholdingProcess, itkFiltersProcessBase)


