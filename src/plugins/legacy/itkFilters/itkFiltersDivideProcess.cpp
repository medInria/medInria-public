/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>

#include <itkFiltersDivideProcess.h>
#include <itkDivideImageFilter.h>
#include <itkImage.h>

#include <medUtilities.h>
#include <medUtilitiesITK.h>

class itkFiltersDivideProcessPrivate
{
public:
    double divideFactor;
};

const double itkFiltersDivideProcess::defaultDivideFactor = 2.0;

//-------------------------------------------------------------------------------------------

itkFiltersDivideProcess::itkFiltersDivideProcess(itkFiltersDivideProcess *parent) 
    : itkFiltersProcessBase(parent), d(new itkFiltersDivideProcessPrivate)
{  
    d->divideFactor = defaultDivideFactor;
}

itkFiltersDivideProcess::itkFiltersDivideProcess(const itkFiltersDivideProcess& other)
     : itkFiltersProcessBase(other), d(other.d)
{
}

//-------------------------------------------------------------------------------------------

itkFiltersDivideProcess::~itkFiltersDivideProcess()
{
    delete d;
}

//-------------------------------------------------------------------------------------------

bool itkFiltersDivideProcess::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkDivideProcess", createitkFiltersDivideProcess);
}

//-------------------------------------------------------------------------------------------

QString itkFiltersDivideProcess::description() const
{
    return tr("Divide by constant filter");
}

//-------------------------------------------------------------------------------------------

void itkFiltersDivideProcess::setParameter(double data)
{
    d->divideFactor = data;
}

//-------------------------------------------------------------------------------------------

int itkFiltersDivideProcess::tryUpdate()
{
    int res = medAbstractProcessLegacy::FAILURE;
    
    if (getInputData())
    {
        res = DISPATCH_ON_3D_PIXEL_TYPE(&itkFiltersDivideProcess::updateProcess, this, getInputData());
    }

    return res;
}

template <class ImageType>
int itkFiltersDivideProcess::updateProcess(medAbstractData *inputData)
{
    typename ImageType::Pointer inputImage = static_cast<ImageType*>(inputData->data());

    typedef itk::DivideImageFilter< ImageType, itk::Image<double, ImageType::ImageDimension>, ImageType >  DivideFilterType;
    typename DivideFilterType::Pointer divideFilter = DivideFilterType::New();

    divideFilter->SetInput(inputImage);
    divideFilter->SetConstant(d->divideFactor);

    itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
    callback->SetClientData(( void * ) this);
    callback->SetCallback(itkFiltersProcessBase::eventCallback);
    divideFilter->AddObserver(itk::ProgressEvent(), callback);

    divideFilter->Update();

    getOutputData()->setData(divideFilter->GetOutput());

    QString newSeriesDescription = "divide filter " + QString::number(d->divideFactor);
    medUtilities::setDerivedMetaData(getOutputData(), inputData, newSeriesDescription);

    return medAbstractProcessLegacy::SUCCESS;
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersDivideProcess()
{
    return new itkFiltersDivideProcess;
}
