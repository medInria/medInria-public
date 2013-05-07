/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#ifndef ITKFILTERSDIVIDEPROCESS_P_H
#define ITKFILTERSDIVIDEPROCESS_P_H

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkSmartPointer.h>

#include "itkFiltersProcessBase_p.h"

#include <medMetaDataKeys.h>

#include "itkImage.h"
#include "itkCommand.h"
#include "itkDivideByConstantImageFilter.h"

class itkFiltersDivideProcess;

class itkFiltersDivideProcessPrivate : public itkFiltersProcessBasePrivate
{
public:
    itkFiltersDivideProcessPrivate(itkFiltersDivideProcess *q = 0) : itkFiltersProcessBasePrivate(q) {}
    itkFiltersDivideProcessPrivate(const itkFiltersDivideProcessPrivate& other) : itkFiltersProcessBasePrivate(other) {}

    virtual ~itkFiltersDivideProcessPrivate(void) {}
    
    double divideFactor;
    
    template <class PixelType> void update ( void )
    {
        typedef itk::Image< PixelType, 3 > ImageType;
        typedef itk::DivideByConstantImageFilter< ImageType, double, ImageType >  DivideFilterType;
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
    
        output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    }
};

DTK_IMPLEMENT_PRIVATE(itkFiltersDivideProcess, itkFiltersProcessBase)


#endif // ITKFILTERSDIVIDEPROCESS_P_H
