/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#ifndef ITKFILTERSNORMALIZEPROCESS_P_H
#define ITKFILTERSNORMALIZEPROCESS_P_H

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkSmartPointer.h>

#include "itkFiltersProcessBase_p.h"

#include <medMetaDataKeys.h>

#include "itkImage.h"
#include "itkCommand.h"
#include "itkNormalizeImageFilter.h"

class itkFiltersNormalizeProcess;

class itkFiltersNormalizeProcessPrivate : public itkFiltersProcessBasePrivate
{
public:
    itkFiltersNormalizeProcessPrivate(itkFiltersNormalizeProcess *q = 0) : itkFiltersProcessBasePrivate(q) {}
    itkFiltersNormalizeProcessPrivate(const itkFiltersNormalizeProcessPrivate& other) : itkFiltersProcessBasePrivate(other) {}

    virtual ~itkFiltersNormalizeProcessPrivate(void) {}
    
    template <class PixelType> void update ( void )
    {
        typedef itk::Image< PixelType, 3 > ImageType;
        typedef itk::NormalizeImageFilter< ImageType, ImageType >  NormalizeFilterType;
        typename NormalizeFilterType::Pointer normalizeFilter = NormalizeFilterType::New();
    
        normalizeFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
        
        callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) this );
        callback->SetCallback ( itkFiltersNormalizeProcessPrivate::eventCallback );
    
        normalizeFilter->AddObserver ( itk::ProgressEvent(), callback );
    
        normalizeFilter->Update();
        output->setData ( normalizeFilter->GetOutput() );
        
        //Set output description metadata
        QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " normalize filter";
    
        output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    }
    
};

DTK_IMPLEMENT_PRIVATE(itkFiltersNormalizeProcess, itkFiltersProcessBase)


#endif // ITKFILTERSNORMALIZEPROCESS_P_H
