/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#ifndef ITKFILTERSMULTIPLYPROCESS_P_H
#define ITKFILTERSMULTIPLYPROCESS_P_H


#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkSmartPointer.h>

#include "itkFiltersProcessBase_p.h"

#include <medMetaDataKeys.h>

#include "itkImage.h"
#include "itkCommand.h"
#include "itkMultiplyByConstantImageFilter.h"

class itkFiltersMultiplyProcess;

class itkFiltersMultiplyProcessPrivate : public itkFiltersProcessBasePrivate
{
public:
    itkFiltersMultiplyProcessPrivate(itkFiltersMultiplyProcess *q = 0) : itkFiltersProcessBasePrivate(q) {}
    itkFiltersMultiplyProcessPrivate(const itkFiltersMultiplyProcessPrivate& other) : itkFiltersProcessBasePrivate(other) {}

    virtual ~itkFiltersMultiplyProcessPrivate(void) {}
    
    double multiplyFactor;
    
    template <class PixelType> void update ( void )
    {
        typedef itk::Image< PixelType, 3 > ImageType;
        typedef itk::MultiplyByConstantImageFilter< ImageType, double, ImageType >  MultiplyFilterType;
        typename MultiplyFilterType::Pointer multiplyFilter = MultiplyFilterType::New();
    
        multiplyFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
        multiplyFilter->SetConstant ( multiplyFactor );
        
        callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) this );
        callback->SetCallback ( itkFiltersMultiplyProcessPrivate::eventCallback );
    
        multiplyFilter->AddObserver ( itk::ProgressEvent(), callback );
    
        multiplyFilter->Update();
        output->setData ( multiplyFilter->GetOutput() );
        
        //Set output description metadata
        QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " multiply filter (" + QString::number(multiplyFactor) + ")";
    
        output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    }

};

DTK_IMPLEMENT_PRIVATE(itkFiltersMultiplyProcess, itkFiltersProcessBase)

#endif // ITKFILTERSMULTIPLYPROCESS_P_H
