/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <dtkCoreSupport/dtkAbstractProcessFactory.h>

#include <itkBinaryThresholdImageFilter.h>
#include <itkFiltersBinaryThresholdingProcess.h>
#include <itkImage.h>

#include <medUtilities.h>
#include <medUtilitiesITK.h>

class itkFiltersBinaryThresholdingProcessPrivate
{
public:
    double lowerValue, upperValue;
    int insideValue, outsideValue;
};

const double itkFiltersBinaryThresholdingProcess::defaultInsideValue = 1.0;

itkFiltersBinaryThresholdingProcess::itkFiltersBinaryThresholdingProcess(itkFiltersBinaryThresholdingProcess *parent)
    : itkFiltersProcessBase(parent), d(new itkFiltersBinaryThresholdingProcessPrivate)
{   
    d->lowerValue = itk::NumericTraits<double>::NonpositiveMin();
    d->upperValue = itk::NumericTraits<double>::max();
    d->insideValue = defaultInsideValue;
    d->outsideValue = 0;
}

itkFiltersBinaryThresholdingProcess::itkFiltersBinaryThresholdingProcess(const itkFiltersBinaryThresholdingProcess& other)
     : itkFiltersProcessBase(other), d(other.d)
{
}

//-------------------------------------------------------------------------------------------

itkFiltersBinaryThresholdingProcess::~itkFiltersBinaryThresholdingProcess()
{
    delete d;
}

//-------------------------------------------------------------------------------------------

bool itkFiltersBinaryThresholdingProcess::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkBinaryThresholdingProcess", createitkFiltersBinaryThresholdingProcess);
}

//-------------------------------------------------------------------------------------------

QString itkFiltersBinaryThresholdingProcess::description() const
{
    return tr("Binary thresholding filter");
}

//-------------------------------------------------------------------------------------------

void itkFiltersBinaryThresholdingProcess::setParameter(int data, int channel)
{
    if (channel == 0)
    {
        d->insideValue = data;
    }
    if (channel == 1)
    {
        d->outsideValue = data;
    }
}

void itkFiltersBinaryThresholdingProcess::setParameter(double data, int channel)
{
    if (channel == 0)
    {
        d->lowerValue = data;
    }
    if (channel == 1)
    {
        d->upperValue = data;
    }
}

//-------------------------------------------------------------------------------------------

int itkFiltersBinaryThresholdingProcess::tryUpdate()
{   
    int res = medAbstractProcessLegacy::FAILURE;

    if (getInputData())
    {
        res = DISPATCH_ON_3D_PIXEL_TYPE(&itkFiltersBinaryThresholdingProcess::updateProcess, this, getInputData());
    }

    return res;
}

template <class ImageType>
int itkFiltersBinaryThresholdingProcess::updateProcess(medAbstractData* inputData)
{
    typename ImageType::Pointer inputImage = static_cast<ImageType*>(inputData->data());

    typedef itk::BinaryThresholdImageFilter < ImageType, ImageType>  BinaryThresholdImageFilterType;
    typename BinaryThresholdImageFilterType::Pointer thresholdFilter = BinaryThresholdImageFilterType::New();
    thresholdFilter->SetInput(inputImage);
    thresholdFilter->SetLowerThreshold(d->lowerValue);
    thresholdFilter->SetUpperThreshold(d->upperValue);
    thresholdFilter->SetInsideValue(d->insideValue);
    thresholdFilter->SetOutsideValue(d->outsideValue);

    itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
    callback->SetClientData(( void * ) this);
    callback->SetCallback(itkFiltersProcessBase::eventCallback);
    thresholdFilter->AddObserver(itk::ProgressEvent(), callback);

    thresholdFilter->Update();

    getOutputData()->setData(thresholdFilter->GetOutput());

    medUtilities::setDerivedMetaData(getOutputData(), inputData, "binaryThreshold");

    return medAbstractProcessLegacy::SUCCESS;
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersBinaryThresholdingProcess()
{
    return new itkFiltersBinaryThresholdingProcess;
}
