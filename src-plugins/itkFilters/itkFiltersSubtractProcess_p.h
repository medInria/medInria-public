/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#ifndef ITKFILTERSSUBTRACTPROCESS_P_H
#define ITKFILTERSSUBTRACTPROCESS_P_H

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkSmartPointer.h>

#include "itkFiltersProcessBase_p.h"

#include <medMetaDataKeys.h>

#include "itkImage.h"
#include "itkCommand.h"
#include "itkSubtractConstantFromImageFilter.h"

class itkFiltersSubtractProcess;

class itkFiltersSubtractProcessPrivate : public itkFiltersProcessBasePrivate
{
public:
    itkFiltersSubtractProcessPrivate(itkFiltersSubtractProcess *q = 0) : itkFiltersProcessBasePrivate(q) {}
    itkFiltersSubtractProcessPrivate(const itkFiltersSubtractProcessPrivate& other) : itkFiltersProcessBasePrivate(other) {}

    virtual ~itkFiltersSubtractProcessPrivate(void) {}
    
    double subtractValue;
    
    template <class PixelType> void update ( void )
    {
        typedef itk::Image< PixelType, 3 > ImageType;
        typedef itk::SubtractConstantFromImageFilter< ImageType, double, ImageType >  SubtractFilterType;
        typename SubtractFilterType::Pointer subtractFilter = SubtractFilterType::New();
    
        subtractFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
        subtractFilter->SetConstant ( subtractValue );
        
        callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) this );
        callback->SetCallback ( itkFiltersSubtractProcessPrivate::eventCallback );
    
        subtractFilter->AddObserver ( itk::ProgressEvent(), callback );
    
        subtractFilter->Update();
        output->setData ( subtractFilter->GetOutput() );
        
        //Set output description metadata
        QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " add filter (" + QString::number(subtractValue) + ")";
    
        output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    }
    
};

DTK_IMPLEMENT_PRIVATE(itkFiltersSubtractProcess, itkFiltersProcessBase)


#endif // ITKFILTERSSUBTRACTPROCESS_P_H
