/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <itkMorphologicalFiltersProcessBase_p.h>

#include <itkImage.h>
#include <itkCommand.h>
#include <itkGrayscaleMorphologicalClosingImageFilter.h>
#include <medMetaDataKeys.h>

class itkFiltersCloseProcess;

class itkFiltersCloseProcessPrivate : public itkMorphologicalFiltersProcessBasePrivate
{
public:
    itkFiltersCloseProcessPrivate(itkFiltersCloseProcess *q = 0) : itkMorphologicalFiltersProcessBasePrivate(q) {}
    itkFiltersCloseProcessPrivate(const itkFiltersCloseProcessPrivate& other) : itkMorphologicalFiltersProcessBasePrivate(other) {}

    virtual ~itkFiltersCloseProcessPrivate(void) {}

    template <class PixelType> void update ( void )
    {
        typedef itk::Image< PixelType, 3 > ImageType;
        
        if(!isRadiusInPixels)
        {
            convertMmInPixels<ImageType>();
        }

        typedef itk::FlatStructuringElement < 3> StructuringElementType;
        StructuringElementType::RadiusType elementRadius;
        elementRadius[0] = radius[0];
        elementRadius[1] = radius[1];
        elementRadius[2] = radius[2];
        StructuringElementType ball = StructuringElementType::Ball(elementRadius);

        typedef itk::GrayscaleMorphologicalClosingImageFilter< ImageType, ImageType, StructuringElementType >  CloseType;
        typename CloseType::Pointer closeFilter = CloseType::New();
        closeFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
        closeFilter->SetKernel ( ball );
        
        callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) this );
        callback->SetCallback ( itkMorphologicalFiltersProcessBasePrivate::eventCallback );
        
        closeFilter->AddObserver ( itk::ProgressEvent(), callback );
        
        closeFilter->Update();
        output->setData ( closeFilter->GetOutput() );
        
        // Add description on output data
        QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );

        newSeriesDescription += " Close filter\n("+ QString::number(radiusMm[0])+", "+
                QString::number(radiusMm[1])+", "+ QString::number(radiusMm[2]);
        isRadiusInPixels? newSeriesDescription += " px": newSeriesDescription += " mm";
        
        output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    }
};

DTK_IMPLEMENT_PRIVATE(itkFiltersCloseProcess, itkMorphologicalFiltersProcessBase)
