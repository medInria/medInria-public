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
#include <itkShrinkImageFilter.h>

class itkFiltersShrinkProcess;

class itkFiltersShrinkProcessPrivate : public itkFiltersProcessBasePrivate
{
public:
    itkFiltersShrinkProcessPrivate(itkFiltersShrinkProcess *q = 0) : itkFiltersProcessBasePrivate(q) {}
    itkFiltersShrinkProcessPrivate(const itkFiltersShrinkProcessPrivate& other) : itkFiltersProcessBasePrivate(other) {}

    virtual ~itkFiltersShrinkProcessPrivate(void) {}
    
    unsigned int shrinkFactors[3];
    
    template <class PixelType> void update ( void )
    {
        typedef itk::Image< PixelType, 3 > ImageType;
        typedef itk::ShrinkImageFilter< ImageType, ImageType >  ShrinkFilterType;
        typename ShrinkFilterType::Pointer shrinkFilter = ShrinkFilterType::New();
        
        shrinkFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
        shrinkFilter->SetShrinkFactors(shrinkFactors);
        
        callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) this );
        callback->SetCallback ( itkFiltersShrinkProcessPrivate::eventCallback );
    
        shrinkFilter->AddObserver ( itk::ProgressEvent(), callback );
    
        shrinkFilter->Update();
        output->setData ( shrinkFilter->GetOutput() );
        
        //Set output description metadata
        QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " shrink filter (" + QString::number(shrinkFactors[0]) + "," + QString::number(shrinkFactors[1]) + "," + QString::number(shrinkFactors[2]) + ")";

        output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    }
    
};

DTK_IMPLEMENT_PRIVATE(itkFiltersShrinkProcess, itkFiltersProcessBase)


