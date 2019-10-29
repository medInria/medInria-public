/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersWindowingProcess.h>

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>

#include <itkImage.h>
#include <itkIntensityWindowingImageFilter.h>
#include <medUtilities.h>

#include <medUtilitiesITK.h>

class itkFiltersWindowingProcessPrivate
{
public:
    double minimumIntensityValue;
    double maximumIntensityValue;
    double minimumOutputIntensityValue;
    double maximumOutputIntensityValue;
};

const double itkFiltersWindowingProcess::defaultMinimumIntensityValue = 0.0;
const double itkFiltersWindowingProcess::defaultMaximumIntensityValue = 255.0;

//-------------------------------------------------------------------------------------------

itkFiltersWindowingProcess::itkFiltersWindowingProcess(itkFiltersWindowingProcess *parent) 
    : itkFiltersProcessBase(parent), d(new itkFiltersWindowingProcessPrivate)
{   
    d->minimumIntensityValue = defaultMinimumIntensityValue;
    d->maximumIntensityValue = defaultMaximumIntensityValue;
    d->minimumOutputIntensityValue = defaultMinimumIntensityValue;
    d->maximumOutputIntensityValue = defaultMaximumIntensityValue;
}

itkFiltersWindowingProcess::itkFiltersWindowingProcess(const itkFiltersWindowingProcess& other)
     : itkFiltersProcessBase(other), d(other.d)
{

}

//-------------------------------------------------------------------------------------------

itkFiltersWindowingProcess::~itkFiltersWindowingProcess()
{
    delete d;
}

//-------------------------------------------------------------------------------------------

bool itkFiltersWindowingProcess::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkWindowingProcess", createitkFiltersWindowingProcess);
}

//-------------------------------------------------------------------------------------------

QString itkFiltersWindowingProcess::description() const
{
    return tr("Intensity windowing filter");
}

//-------------------------------------------------------------------------------------------

void itkFiltersWindowingProcess::setParameter(double data, int channel)
{
    switch (channel)
    {
        case 0:
            d->minimumIntensityValue = data;
            break;
        case 1:
            d->maximumIntensityValue = data;
            break;
        case 2:
            d->minimumOutputIntensityValue = data;
            break;
        case 3:
            d->maximumOutputIntensityValue = data;
            break;
        default:
            break;
    }
}

//-------------------------------------------------------------------------------------------

int itkFiltersWindowingProcess::tryUpdate()
{   
    int res = medAbstractProcessLegacy::FAILURE;

    if (getInputData())
    {
        res = DISPATCH_ON_3D_PIXEL_TYPE(&itkFiltersWindowingProcess::updateProcess, this, getInputData());
    }

    return res;
}

template <class ImageType>
int itkFiltersWindowingProcess::updateProcess(medAbstractData* inputData)
{
    typename ImageType::Pointer inputImage = static_cast<ImageType*>(inputData->data());

    typedef itk::IntensityWindowingImageFilter< ImageType, ImageType >  WindowingFilterType;
    typename WindowingFilterType::Pointer windowingFilter = WindowingFilterType::New();

    windowingFilter->SetInput(inputImage);
    windowingFilter->SetWindowMinimum((typename ImageType::PixelType) d->minimumIntensityValue);
    windowingFilter->SetWindowMaximum((typename ImageType::PixelType) d->maximumIntensityValue);
    windowingFilter->SetOutputMinimum((typename ImageType::PixelType) d->minimumOutputIntensityValue);
    windowingFilter->SetOutputMaximum((typename ImageType::PixelType) d->maximumOutputIntensityValue);

    itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
    callback->SetClientData(( void * ) this);
    callback->SetCallback(itkFiltersProcessBase::eventCallback);
    windowingFilter->AddObserver(itk::ProgressEvent(), callback);

    windowingFilter->Update();

    getOutputData()->setData(windowingFilter->GetOutput());

    // Set output description metadata
    QString newSeriesDescription = "windowing " + QString::number(d->minimumIntensityValue)
            + " " + QString::number(d->maximumIntensityValue);
    medUtilities::setDerivedMetaData(getOutputData(), inputData, newSeriesDescription);

    return medAbstractProcessLegacy::SUCCESS;
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersWindowingProcess()
{
    return new itkFiltersWindowingProcess;
}
