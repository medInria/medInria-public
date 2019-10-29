/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkMorphologicalFiltersProcessBase.h"

#include <itkBinaryDilateImageFilter.h>
#include <itkBinaryErodeImageFilter.h>
#include <itkBinaryMorphologicalClosingImageFilter.h>
#include <itkBinaryMorphologicalOpeningImageFilter.h>
#include <itkFlatStructuringElement.h>
#include <itkGrayscaleMorphologicalClosingImageFilter.h>
#include <itkGrayscaleMorphologicalOpeningImageFilter.h>
#include <itkImage.h>
#include <itkMinimumMaximumImageFilter.h>
#include <medUtilities.h>
#include <medUtilitiesITK.h>

class itkMorphologicalFiltersProcessBasePrivate
{
public:
    double radius[3];
    double radiusMm[3];
    bool isRadiusInPixels;
    itkMorphologicalFiltersProcessBase::KernelShape kernelShape;
};

itkMorphologicalFiltersProcessBase::itkMorphologicalFiltersProcessBase(itkMorphologicalFiltersProcessBase *parent) 
    : itkFiltersProcessBase(parent), d(new itkMorphologicalFiltersProcessBasePrivate)
{  
    d->radius[0] = 0;
    d->radius[1] = 0;
    d->radius[2] = 0;

    d->radiusMm[0] = 0;
    d->radiusMm[1] = 0;
    d->radiusMm[2] = 0;

    d->isRadiusInPixels = false;

    d->kernelShape = itkMorphologicalFiltersProcessBase::BallKernel;
}

itkMorphologicalFiltersProcessBase::itkMorphologicalFiltersProcessBase(const itkMorphologicalFiltersProcessBase& other)
     : itkFiltersProcessBase(other), d(other.d)
{
}

void itkMorphologicalFiltersProcessBase::setParameter(double data, int channel)
{
    if (channel <= 1)
    {
        d->radius[0] = data;
        d->radius[1] = data;
        d->radius[2] = data;

        if (channel == 1) // data is in pixels
        {
            d->isRadiusInPixels = true;
        }
        if (channel == 0) // data is in mm
        {
            d->isRadiusInPixels = false;
        }
    }
}

void itkMorphologicalFiltersProcessBase::setParameter(int data)
{
    d->kernelShape = static_cast<KernelShape>(data);
}

// Only called if not defined in subclasses (e.g. dilate/erodeFilter)
int itkMorphologicalFiltersProcessBase::tryUpdate()
{
    int res = medAbstractProcessLegacy::FAILURE;

    if (getInputData())
    {
        res = DISPATCH_ON_3D_PIXEL_TYPE(&itkMorphologicalFiltersProcessBase::updateProcess, this, getInputData());
    }
    return res;
}

template <class ImageType>
void itkMorphologicalFiltersProcessBase::convertMmInPixels(ImageType *image)
{
    for (unsigned int i=0; i<image->GetSpacing().Size(); i++)
    {
        d->radius[i] = floor((d->radius[i]/image->GetSpacing()[i])+0.5); //rounding
        d->radiusMm[i] = d->radius[i] * image->GetSpacing()[i];
    }
}

template <class ImageType>
int itkMorphologicalFiltersProcessBase::updateProcess(medAbstractData *inputData)
{
    typename ImageType::Pointer inputImage = static_cast<ImageType*>(inputData->data());

    if(!d->isRadiusInPixels)
    {
        convertMmInPixels<ImageType>(inputImage);
    }

    typedef itk::FlatStructuringElement < 3> StructuringElementType;
    StructuringElementType::RadiusType elementRadius;
    elementRadius[0] = d->radius[0]; //radius (double) is truncated
    elementRadius[1] = d->radius[1];
    elementRadius[2] = d->radius[2];

    StructuringElementType kernel;

    switch (d->kernelShape)
    {
    case itkMorphologicalFiltersProcessBase::BallKernel:
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
    imageCalculatorFilter->SetInput(inputImage);
    imageCalculatorFilter->Update();

    typedef itk::KernelImageFilter< ImageType, ImageType, StructuringElementType >  FilterType;
    typename FilterType::Pointer filter;

    QString filenameDescription;

    if(description() == "Dilate filter")
    {
        filenameDescription = "dilated";

        typedef itk::BinaryDilateImageFilter< ImageType, ImageType,StructuringElementType >  DilateFilterType;
        filter = DilateFilterType::New();
        dynamic_cast<DilateFilterType *>(filter.GetPointer())->SetForegroundValue(imageCalculatorFilter->GetMaximum());
        dynamic_cast<DilateFilterType *>(filter.GetPointer())->SetBackgroundValue(imageCalculatorFilter->GetMinimum());
    }
    else if(description() == "Erode filter")
    {
        filenameDescription = "eroded";

        typedef itk::BinaryErodeImageFilter< ImageType, ImageType,StructuringElementType >  ErodeFilterType;
        filter = ErodeFilterType::New();
        dynamic_cast<ErodeFilterType *>(filter.GetPointer())->SetForegroundValue(imageCalculatorFilter->GetMaximum());
        dynamic_cast<ErodeFilterType *>(filter.GetPointer())->SetBackgroundValue(imageCalculatorFilter->GetMinimum());
    }
    else if(description() == "Grayscale Close filter")
    {
        filenameDescription = "grayscaleClosed";

        typedef itk::GrayscaleMorphologicalClosingImageFilter< ImageType, ImageType, StructuringElementType >  GCloseFilterType;
        filter = GCloseFilterType::New();
    }
    else if(description() == "Grayscale Open filter")
    {
        filenameDescription = "grayscaleOpened";

        typedef itk::GrayscaleMorphologicalOpeningImageFilter< ImageType, ImageType, StructuringElementType >  GOpenFilterType;
        filter = GOpenFilterType::New();
    }
    else if(description() == "Binary Close filter")
    {
        filenameDescription = "binaryClosed";

        typedef itk::BinaryMorphologicalClosingImageFilter< ImageType, ImageType, StructuringElementType >  BCloseFilterType;
        filter = BCloseFilterType::New();
        dynamic_cast<BCloseFilterType *>(filter.GetPointer())->SetForegroundValue(imageCalculatorFilter->GetMaximum());
    }
    else if(description() == "Binary Open filter")
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
        return medAbstractProcessLegacy::FAILURE;
    }

    filter->SetInput(inputImage);
    filter->SetKernel ( kernel );

    itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
    callback->SetClientData ( ( void * ) this );
    callback->SetCallback ( itkFiltersProcessBase::eventCallback );
    filter->AddObserver ( itk::ProgressEvent(), callback );

    filter->Update();

    getOutputData()->setData ( filter->GetOutput() );

    // Add description on output data
    QString newSeriesDescription = filenameDescription + " ";

    if (d->isRadiusInPixels)
    {
        newSeriesDescription += QString::number(floor(d->radius[0])) + "/" +
                QString::number(floor(d->radius[1])) + "/" +
                QString::number(floor(d->radius[2])) +
                " px";
    }
    else
    {
        newSeriesDescription += QString::number(float(d->radiusMm[0])) + "/" +
                QString::number(float(d->radiusMm[1])) + "/" +
                QString::number(float(d->radiusMm[2])) +
                " mm";
    }

    medUtilities::setDerivedMetaData(getOutputData(), inputData, newSeriesDescription);

    return medAbstractProcessLegacy::SUCCESS;
}
