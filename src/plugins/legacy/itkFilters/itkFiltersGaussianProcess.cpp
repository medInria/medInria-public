/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>

#include <itkFiltersGaussianProcess.h>
#include <itkImage.h>
#include <itkSmoothingRecursiveGaussianImageFilter.h>

#include <medUtilities.h>
#include <medUtilitiesITK.h>

class itkFiltersGaussianProcessPrivate
{
public:
    double sigma;
};

const double itkFiltersGaussianProcess::defaultSigma = 1.0;

itkFiltersGaussianProcess::itkFiltersGaussianProcess(itkFiltersGaussianProcess *parent) 
    : itkFiltersProcessBase(parent), d(new itkFiltersGaussianProcessPrivate)
{
    d->sigma = defaultSigma;
}

itkFiltersGaussianProcess::itkFiltersGaussianProcess(const itkFiltersGaussianProcess& other)
     : itkFiltersProcessBase(other), d(other.d)
{
}

//-------------------------------------------------------------------------------------------

itkFiltersGaussianProcess::~itkFiltersGaussianProcess()
{
    delete d;
}

//-------------------------------------------------------------------------------------------

bool itkFiltersGaussianProcess::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkGaussianProcess", createitkFiltersGaussianProcess);
}

//-------------------------------------------------------------------------------------------

QString itkFiltersGaussianProcess::description() const
{
    return tr("Gaussian filter");
}

//-------------------------------------------------------------------------------------------

void itkFiltersGaussianProcess::setParameter(double data)
{
    d->sigma = data;
}

//-------------------------------------------------------------------------------------------

int itkFiltersGaussianProcess::tryUpdate()
{  
    int res = medAbstractProcessLegacy::FAILURE;

    if (getInputData())
    {
        res = DISPATCH_ON_3D_PIXEL_TYPE(&itkFiltersGaussianProcess::updateProcess, this, getInputData());
    }
    
    return res;
}

template <class ImageType>
int itkFiltersGaussianProcess::updateProcess(medAbstractData *inputData)
{
    typename ImageType::Pointer inputImage = static_cast<ImageType*>(inputData->data());

    typedef itk::SmoothingRecursiveGaussianImageFilter< ImageType, ImageType >  GaussianFilterType;
    typename GaussianFilterType::Pointer gaussianFilter = GaussianFilterType::New();

    gaussianFilter->SetInput(inputImage);
    gaussianFilter->SetSigma(d->sigma);

    itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
    callback->SetClientData(( void * ) this);
    callback->SetCallback(itkFiltersProcessBase::eventCallback);
    gaussianFilter->AddObserver(itk::ProgressEvent(), callback);

    gaussianFilter->Update();

    getOutputData()->setData(gaussianFilter->GetOutput());

    QString newSeriesDescription = "gaussian filter " + QString::number(d->sigma);
    medUtilities::setDerivedMetaData(getOutputData(), inputData, newSeriesDescription);

    return medAbstractProcessLegacy::SUCCESS;
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersGaussianProcess()
{
    return new itkFiltersGaussianProcess;
}
