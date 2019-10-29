/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>

#include <itkFiltersMultiplyProcess.h>
#include <itkImage.h>
#include <itkMultiplyImageFilter.h>

#include <medUtilities.h>
#include <medUtilitiesITK.h>

class itkFiltersMultiplyProcessPrivate
{
public:
    double multiplyFactor;
};

const double itkFiltersMultiplyProcess::defaultMultiplyFactor = 2.0;

//-------------------------------------------------------------------------------------------

itkFiltersMultiplyProcess::itkFiltersMultiplyProcess(itkFiltersMultiplyProcess *parent) 
    : itkFiltersProcessBase(parent), d(new itkFiltersMultiplyProcessPrivate)
{  
    d->multiplyFactor = defaultMultiplyFactor;
}

itkFiltersMultiplyProcess::itkFiltersMultiplyProcess(const itkFiltersMultiplyProcess& other)
     : itkFiltersProcessBase(other), d(other.d)
{
}

//-------------------------------------------------------------------------------------------

itkFiltersMultiplyProcess::~itkFiltersMultiplyProcess()
{
    delete d;
}

//-------------------------------------------------------------------------------------------

bool itkFiltersMultiplyProcess::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkMultiplyProcess", createitkFiltersMultiplyProcess);
}

//-------------------------------------------------------------------------------------------

QString itkFiltersMultiplyProcess::description() const
{
    return tr("Multiply by constant filter");
}

//-------------------------------------------------------------------------------------------

void itkFiltersMultiplyProcess::setParameter(double data)
{
    d->multiplyFactor = data;
}

//-------------------------------------------------------------------------------------------

int itkFiltersMultiplyProcess::tryUpdate()
{  
    int res = medAbstractProcessLegacy::FAILURE;

    if (getInputData())
    {
        res = DISPATCH_ON_3D_PIXEL_TYPE(&itkFiltersMultiplyProcess::updateProcess, this, getInputData());
    }

    return res;
}

template <class ImageType>
int itkFiltersMultiplyProcess::updateProcess(medAbstractData* inputData)
{
    typename ImageType::Pointer inputImage = static_cast<ImageType*>(inputData->data());

    typedef itk::MultiplyImageFilter< ImageType, itk::Image<double, ImageType::ImageDimension>, ImageType >  MultiplyFilterType;
    typename MultiplyFilterType::Pointer multiplyFilter = MultiplyFilterType::New();

    multiplyFilter->SetInput(inputImage);
    multiplyFilter->SetConstant(d->multiplyFactor);

    itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
    callback->SetClientData(( void * ) this);
    callback->SetCallback(itkFiltersProcessBase::eventCallback);
    multiplyFilter->AddObserver(itk::ProgressEvent(), callback);

    multiplyFilter->Update();

    getOutputData()->setData(multiplyFilter->GetOutput());

    // Set output description metadata
    QString newSeriesDescription = "multiply filter " + QString::number(d->multiplyFactor);
    medUtilities::setDerivedMetaData(getOutputData(), inputData, newSeriesDescription);

    return medAbstractProcessLegacy::SUCCESS;
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersMultiplyProcess()
{
    return new itkFiltersMultiplyProcess;
}
