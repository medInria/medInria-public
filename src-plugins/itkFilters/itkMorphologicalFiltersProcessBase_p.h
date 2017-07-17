/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <itkFiltersProcessBase.h>
#include <itkFiltersProcessBase_p.h>
#include <medAbstractImageData.h>
#include <medDataManager.h>
#include <medMetaDataKeys.h>
#include <dtkLog/dtkLog.h>

#include <itkFiltersPluginExport.h>

#include <itkBinaryDilateImageFilter.h>
#include <itkBinaryErodeImageFilter.h>
#include <itkBinaryMorphologicalClosingImageFilter.h>
#include <itkBinaryMorphologicalOpeningImageFilter.h>
#include <itkFlatStructuringElement.h>
#include <itkGrayscaleMorphologicalClosingImageFilter.h>
#include <itkGrayscaleMorphologicalOpeningImageFilter.h>
#include <itkKernelImageFilter.h>
#include <itkCommand.h>
#include <itkImage.h>
#include <itkMinimumMaximumImageFilter.h>
#include <itkProcessObject.h>
#include <medUtilities.h>

class itkMorphologicalFiltersProcessBase;

class ITKFILTERSPLUGIN_EXPORT itkMorphologicalFiltersProcessBasePrivate : public itkFiltersProcessBasePrivate
{
public:
    itkMorphologicalFiltersProcessBasePrivate(itkMorphologicalFiltersProcessBase *q = 0) : itkFiltersProcessBasePrivate(q) {}
    itkMorphologicalFiltersProcessBasePrivate(const itkMorphologicalFiltersProcessBasePrivate& other) : itkFiltersProcessBasePrivate(other) {}

    virtual ~itkMorphologicalFiltersProcessBasePrivate(void) {}

    double radius[3];
    double radiusMm[3];
    bool isRadiusInPixels;
    itkMorphologicalFiltersProcessBase::KernelShape kernelShape;
	
    template <class ImageType> void convertMmInPixels ( void )
    {
        ImageType *image = dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) );
        for (unsigned int i=0; i<image->GetSpacing().Size(); i++)
        {
            radius[i] = floor((radius[i]/image->GetSpacing()[i])+0.5); //rounding
            radiusMm[i] = radius[i] * image->GetSpacing()[i];
        }
    }


    template <class PixelType> int update ( void )
    {
        typedef itk::Image< PixelType, 3 > ImageType;

        if(!isRadiusInPixels)
        {
            convertMmInPixels<ImageType>();
        }

        typedef itk::FlatStructuringElement < 3> StructuringElementType;
        StructuringElementType::RadiusType elementRadius;
        elementRadius[0] = radius[0]; //radius (double) is truncated
        elementRadius[1] = radius[1];
        elementRadius[2] = radius[2];

        StructuringElementType kernel;

        switch (kernelShape)
        {
        case itkMorphologicalFiltersProcessBase::BallKernel: default:
            kernel = StructuringElementType::Ball(elementRadius);
            break;
        case itkMorphologicalFiltersProcessBase::CrossKernel:
            kernel = StructuringElementType::Cross(elementRadius);
            break;
        case itkMorphologicalFiltersProcessBase::BoxKernel:
            kernel = StructuringElementType::Box(elementRadius);
            break;
        }

        kernel.SetRadiusIsParametric(true);

        typedef itk::MinimumMaximumImageFilter <ImageType> ImageCalculatorFilterType;
        typename ImageCalculatorFilterType::Pointer imageCalculatorFilter = ImageCalculatorFilterType::New();
        imageCalculatorFilter->SetInput( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
        imageCalculatorFilter->Update();

        typedef itk::KernelImageFilter< ImageType, ImageType, StructuringElementType >  FilterType;
        typename FilterType::Pointer filter;

        QString filenameDescription;

        if(description == "Dilate filter")
        {
            filenameDescription = "dilated";

            typedef itk::BinaryDilateImageFilter< ImageType, ImageType,StructuringElementType >  DilateFilterType;
            filter = DilateFilterType::New();
            dynamic_cast<DilateFilterType *>(filter.GetPointer())->SetForegroundValue(imageCalculatorFilter->GetMaximum());
            dynamic_cast<DilateFilterType *>(filter.GetPointer())->SetBackgroundValue(imageCalculatorFilter->GetMinimum());
        }
        else if(description == "Erode filter")
        {
            filenameDescription = "eroded";

            typedef itk::BinaryErodeImageFilter< ImageType, ImageType,StructuringElementType >  ErodeFilterType;
            filter = ErodeFilterType::New();
            dynamic_cast<ErodeFilterType *>(filter.GetPointer())->SetForegroundValue(imageCalculatorFilter->GetMaximum());
            dynamic_cast<ErodeFilterType *>(filter.GetPointer())->SetBackgroundValue(imageCalculatorFilter->GetMinimum());
        }
        else if(description == "Grayscale Close filter")
        {
            filenameDescription = "grayscaleClosed";

            typedef itk::GrayscaleMorphologicalClosingImageFilter< ImageType, ImageType, StructuringElementType >  GCloseFilterType;
            filter = GCloseFilterType::New();
        }
        else if(description == "Grayscale Open filter")
        {
            filenameDescription = "grayscaleOpened";

            typedef itk::GrayscaleMorphologicalOpeningImageFilter< ImageType, ImageType, StructuringElementType >  GOpenFilterType;
            filter = GOpenFilterType::New();
        }
        else if(description == "Binary Close filter")
        {
            filenameDescription = "binaryClosed";

            typedef itk::BinaryMorphologicalClosingImageFilter< ImageType, ImageType, StructuringElementType >  BCloseFilterType;
            filter = BCloseFilterType::New();
            dynamic_cast<BCloseFilterType *>(filter.GetPointer())->SetForegroundValue(imageCalculatorFilter->GetMaximum());
        }
        else if(description == "Binary Open filter")
        {
            filenameDescription = "binaryOpened";

            typedef itk::BinaryMorphologicalOpeningImageFilter< ImageType, ImageType, StructuringElementType >  BOpenFilterType;
            filter = BOpenFilterType::New();
            dynamic_cast<BOpenFilterType *>(filter.GetPointer())->SetForegroundValue(imageCalculatorFilter->GetMaximum());
            dynamic_cast<BOpenFilterType *>(filter.GetPointer())->SetBackgroundValue(imageCalculatorFilter->GetMinimum());
        }
        else
        {
            qDebug()<<"Wrong morphological filter";
            return DTK_FAILURE;
        }

        filter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
        filter->SetKernel ( kernel );

        callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) this );
        callback->SetCallback ( itkMorphologicalFiltersProcessBasePrivate::eventCallback );

        filter->AddObserver ( itk::ProgressEvent(), callback );

        filter->Update();

        output->setData ( filter->GetOutput() );

        // Add description on output data
        QString newSeriesDescription = filenameDescription + " ";

        if (isRadiusInPixels)
        {
            newSeriesDescription += QString::number(floor(radius[0])) + "/" +
                    QString::number(floor(radius[1])) + "/" +
                    QString::number(floor(radius[2])) +
                    " px";
        }
        else
        {
            newSeriesDescription += QString::number(float(radiusMm[0])) + "/" +
                    QString::number(float(radiusMm[1])) + "/" +
                    QString::number(float(radiusMm[2])) +
                    " mm";
        }

        medUtilities::setDerivedMetaData(output, input, newSeriesDescription);

        return DTK_SUCCEED;
    }
};

DTK_IMPLEMENT_PRIVATE(itkMorphologicalFiltersProcessBase, itkFiltersProcessBase)


