/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>

#include <itkFiltersMedianProcess.h>
#include <itkImage.h>
#include <itkMedianImageFilter.h>

#include <medUtilities.h>
#include <medUtilitiesITK.h>

class itkFiltersMedianProcessPrivate
{
public:
    double medianSize;
};

const double itkFiltersMedianProcess::defaultMedianSize = 1.0;

//-------------------------------------------------------------------------------------------

itkFiltersMedianProcess::itkFiltersMedianProcess(itkFiltersMedianProcess *parent) 
    : itkFiltersProcessBase(parent), d(new itkFiltersMedianProcessPrivate)
{
    d->medianSize = defaultMedianSize;
}

//-------------------------------------------------------------------------------------------

bool itkFiltersMedianProcess::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkMedianProcess", createitkFiltersMedianProcess);
}

//-------------------------------------------------------------------------------------------

QString itkFiltersMedianProcess::description() const
{
    return tr("Median filter");
}

//-------------------------------------------------------------------------------------------

void itkFiltersMedianProcess::setParameter(double data)
{
    d->medianSize = data;
}

//-------------------------------------------------------------------------------------------

int itkFiltersMedianProcess::tryUpdate()
{
    int res = medAbstractProcessLegacy::FAILURE;

    if (getInputData())
    {
        res = DISPATCH_ON_3D_PIXEL_TYPE(&itkFiltersMedianProcess::updateProcess, this, getInputData());
    }

    return res;
}

template <class ImageType>
int itkFiltersMedianProcess::updateProcess(medAbstractData* inputData)
{
    typename ImageType::Pointer inputImage = static_cast<ImageType*>(inputData->data());

    typedef itk::MedianImageFilter< ImageType, ImageType >  MedianFilterType;
    typename MedianFilterType::Pointer medianFilter = MedianFilterType::New();
    typename MedianFilterType::InputSizeType radius;

    radius.Fill(d->medianSize);

    medianFilter->SetRadius(radius);
    medianFilter->SetInput(inputImage);

    itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
    callback->SetClientData(( void * ) this);
    callback->SetCallback(itkFiltersProcessBase::eventCallback);
    medianFilter->AddObserver(itk::ProgressEvent(), callback);

    medianFilter->Update();

    getOutputData()->setData(medianFilter->GetOutput());

    // Set output description metadata
    QString newSeriesDescription = "median filter " + QString::number(d->medianSize);
    medUtilities::setDerivedMetaData(getOutputData(), inputData, newSeriesDescription);

    return medAbstractProcessLegacy::SUCCESS;
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersMedianProcess()
{
    return new itkFiltersMedianProcess;
}
