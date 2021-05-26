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
#include <itkCastImageFilter.h>
#include <itkConnectedComponentImageFilter.h>
#include <itkFiltersComponentSizeThresholdProcess.h>
#include <itkRelabelComponentImageFilter.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>
#include <medUtilities.h>
#include <medUtilitiesITK.h>

class itkFiltersComponentSizeThresholdProcessPrivate
{
public:
    double minimumSize;
    bool binarize;
};

const int itkFiltersComponentSizeThresholdProcess::defaultMinimumSize = 50;
const bool itkFiltersComponentSizeThresholdProcess::defaultBinarize = true;

//-------------------------------------------------------------------------------------------

itkFiltersComponentSizeThresholdProcess::itkFiltersComponentSizeThresholdProcess(itkFiltersComponentSizeThresholdProcess *parent) 
    : itkFiltersProcessBase(parent), d(new itkFiltersComponentSizeThresholdProcessPrivate)
{  
    d->minimumSize = defaultMinimumSize;
    d->binarize = defaultBinarize;
}

itkFiltersComponentSizeThresholdProcess::itkFiltersComponentSizeThresholdProcess(const itkFiltersComponentSizeThresholdProcess& other)
     : itkFiltersProcessBase(other), d(other.d)
{
}

//-------------------------------------------------------------------------------------------

itkFiltersComponentSizeThresholdProcess::~itkFiltersComponentSizeThresholdProcess()
{
    delete d;
}

//-------------------------------------------------------------------------------------------

bool itkFiltersComponentSizeThresholdProcess::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkComponentSizeThresholdProcess", createitkFiltersComponentSizeThresholdProcess);
}

QString itkFiltersComponentSizeThresholdProcess::description() const
{
    return tr("Size Threshold filter");
}

//-------------------------------------------------------------------------------------------

void itkFiltersComponentSizeThresholdProcess::setParameter(int data, int channel)
{
    switch(channel)
    {
    case 1:
        d->minimumSize = data;
        break;
    case 2:
        d->binarize = static_cast<bool>(data);
        break;
    default:
        qDebug()<< metaObject()->className()
                << "::set parameter - unknown channel.";
    }
}

//-------------------------------------------------------------------------------------------

int itkFiltersComponentSizeThresholdProcess::tryUpdate()
{  
    int res = medAbstractProcessLegacy::FAILURE;

    if (getInputData())
    {
        res = DISPATCH_ON_3D_PIXEL_TYPE(&itkFiltersComponentSizeThresholdProcess::updateProcess, this, getInputData());
    }

    return res;
}

template <class InputImageType>
dtkSmartPointer<medAbstractData> itkFiltersComponentSizeThresholdProcess::castToOutputType(medAbstractData *inputData)
{
    // We will later label the image so we don't care about precision.
    typedef itk::CastImageFilter< InputImageType, OutputImageType > CastFilterType;

    typename CastFilterType::Pointer  caster = CastFilterType::New();
    typename InputImageType::Pointer im = static_cast<InputImageType*>(inputData->data());
    caster->SetInput(im);
    caster->Update();

    dtkSmartPointer<medAbstractData> outputData = medAbstractDataFactory::instance()->createSmartPointer(medUtilitiesITK::itkDataImageId<OutputImageType>());
    outputData->setData(caster->GetOutput());

    return outputData;
}

template <class InputImageType>
int itkFiltersComponentSizeThresholdProcess::updateProcess(medAbstractData *inputData)
{
    dtkSmartPointer<medAbstractData> adjustedInputData = inputData;

    if (std::is_floating_point<typename InputImageType::PixelType>::value)
    {
        adjustedInputData = castToOutputType<InputImageType>(inputData);;
    }

    typename InputImageType::Pointer inputImage = static_cast<InputImageType*>(inputData->data());

    typedef itk::ConnectedComponentImageFilter <InputImageType, OutputImageType> ConnectedComponentFilterType;
    typename ConnectedComponentFilterType::Pointer connectedComponentFilter = ConnectedComponentFilterType::New();
    connectedComponentFilter->SetInput(inputImage);
    connectedComponentFilter->Update();

    // RELABEL COMPONENTS according to their sizes (0:largest(background))
    typedef itk::RelabelComponentImageFilter<OutputImageType, OutputImageType> FilterType;
    typename FilterType::Pointer relabelFilter = FilterType::New();
    relabelFilter->SetInput(connectedComponentFilter->GetOutput());
    relabelFilter->SetMinimumObjectSize(d->minimumSize);
    relabelFilter->Update();

    itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
    callback->SetClientData ( ( void * ) this );
    callback->SetCallback ( itkFiltersProcessBase::eventCallback );
    connectedComponentFilter->AddObserver ( itk::ProgressEvent(), callback );

    setOutputData(medAbstractDataFactory::instance()->createSmartPointer(medUtilitiesITK::itkDataImageId<OutputImageType>()));

    if (d->binarize == true)
    {

        // BINARY FILTER
        typedef itk::BinaryThresholdImageFilter <OutputImageType, OutputImageType> BinaryThresholdImageFilterType;
        typename BinaryThresholdImageFilterType::Pointer thresholdFilter
                = BinaryThresholdImageFilterType::New();
        thresholdFilter->SetInput(relabelFilter->GetOutput());
        thresholdFilter->SetUpperThreshold(0);
        thresholdFilter->SetInsideValue(0);
        thresholdFilter->SetOutsideValue(1);

        thresholdFilter->Update();
        getOutputData()->setData(thresholdFilter->GetOutput());
    }
    else
    {
        getOutputData()->setData( relabelFilter->GetOutput());
    }

    QString newSeriesDescription = "connectedComponent " + QString::number(d->minimumSize);
    medUtilities::setDerivedMetaData(getOutputData(), inputData, newSeriesDescription);

    return medAbstractProcessLegacy::SUCCESS;
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersComponentSizeThresholdProcess()
{
    return new itkFiltersComponentSizeThresholdProcess;
}
