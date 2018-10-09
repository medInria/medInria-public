/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersGaussianProcess.h>

#include <dtkCore/dtkAbstractProcessFactory.h>

#include <itkImage.h>
#include <itkSmoothingRecursiveGaussianImageFilter.h>

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

itkFiltersGaussianProcess::~itkFiltersGaussianProcess( void )
{
    delete d;
}

//-------------------------------------------------------------------------------------------

bool itkFiltersGaussianProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkGaussianProcess", createitkFiltersGaussianProcess);
}

//-------------------------------------------------------------------------------------------

QString itkFiltersGaussianProcess::description() const
{
    return tr("ITK gaussian filter");
}

//-------------------------------------------------------------------------------------------

void itkFiltersGaussianProcess::setParameter(double data, int channel)
{
    if (channel != 0)
        return;
    
    d->sigma = data;
}

//-------------------------------------------------------------------------------------------

int itkFiltersGaussianProcess::tryUpdate()
{  
    int res = DTK_FAILURE;

    if ( getInputData() )
    {
        res = DISPATCH_ON_3D_PIXEL_TYPE(&itkFiltersGaussianProcess::updateProcess, this, getInputData());
    }
    
    return res;
}

template <class ImageType>
int itkFiltersGaussianProcess::updateProcess(medAbstractData* inputData)
{
    typename ImageType::Pointer inputImage = static_cast<ImageType*>(inputData->data());

    typedef itk::SmoothingRecursiveGaussianImageFilter< ImageType, ImageType >  GaussianFilterType;
    typename GaussianFilterType::Pointer gaussianFilter = GaussianFilterType::New();

    gaussianFilter->SetInput(inputImage);
    gaussianFilter->SetSigma( d->sigma );

    itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
    callback->SetClientData ( ( void * ) this );
    callback->SetCallback ( itkFiltersProcessBase::eventCallback );
    gaussianFilter->AddObserver ( itk::ProgressEvent(), callback );

    gaussianFilter->Update();

    getOutputData()->setData ( gaussianFilter->GetOutput() );

    QString newSeriesDescription = "gaussian filter " + QString::number(d->sigma);
    medUtilities::setDerivedMetaData(getOutputData(), inputData, newSeriesDescription);

    return DTK_SUCCEED;
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersGaussianProcess ( void )
{
    return new itkFiltersGaussianProcess;
}
