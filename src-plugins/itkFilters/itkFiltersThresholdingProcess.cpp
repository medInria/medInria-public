/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersThresholdingProcess.h>

#include <dtkCore/dtkAbstractProcessFactory.h>

#include <itkImage.h>
#include <itkThresholdImageFilter.h>

#include <medUtilitiesITK.h>

class itkFiltersThresholdingProcessPrivate
{
public:
    double thresholdValue, lowerValue, upperValue;
    int outsideValue;
    //bool comparisonOperator;
    int thresholdType;
};

const double itkFiltersThresholdingProcess::defaultThreshold = 200.0;
const double itkFiltersThresholdingProcess::defaultUpper = itk::NumericTraits<double>::max();
const double itkFiltersThresholdingProcess::defaultLower = itk::NumericTraits<double>::NonpositiveMin();
const int itkFiltersThresholdingProcess::defaultOutsideValue = 0;
const int itkFiltersThresholdingProcess::lowerButtonId = 0;
const int itkFiltersThresholdingProcess::upperButtonId = 1;
const int itkFiltersThresholdingProcess::outsideButtonId = 2;

itkFiltersThresholdingProcess::itkFiltersThresholdingProcess(itkFiltersThresholdingProcess *parent) 
    : itkFiltersProcessBase(parent), d(new itkFiltersThresholdingProcessPrivate)
{   
    d->thresholdValue = defaultThreshold;
    d->lowerValue = defaultLower;
    d->upperValue = defaultUpper;
    d->outsideValue = defaultOutsideValue;
    d->thresholdType = -1;
}

itkFiltersThresholdingProcess::itkFiltersThresholdingProcess(const itkFiltersThresholdingProcess& other)
     : itkFiltersProcessBase(other), d(other.d)
{
}

//-------------------------------------------------------------------------------------------

itkFiltersThresholdingProcess::~itkFiltersThresholdingProcess( void )
{
    delete d;
}

//-------------------------------------------------------------------------------------------

bool itkFiltersThresholdingProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkThresholdingProcess", createitkFiltersThresholdingProcess);
}

//-------------------------------------------------------------------------------------------

QString itkFiltersThresholdingProcess::description() const
{
    return tr("ITK thresholding filter");
}

//-------------------------------------------------------------------------------------------

void itkFiltersThresholdingProcess::setParameter(int data)
{
    d->thresholdType = data;
}

void itkFiltersThresholdingProcess::setParameter(double data, int channel)
{
    if (channel > 3)
        return;
    if (channel == 0)
        d->thresholdValue = data;
    if (channel == 1)
        d->lowerValue = data;
    if (channel == 2)
        d->upperValue = data;
    if (channel == 3)
        d->outsideValue = data;
}

//-------------------------------------------------------------------------------------------

int itkFiltersThresholdingProcess::tryUpdate()
{   
    int res = DTK_FAILURE;

    if ( getInputData() )
    {
        res = DISPATCH_ON_3D_PIXEL_TYPE(&itkFiltersThresholdingProcess::updateProcess, this, getInputData());
    }

    return res;
}

template <class ImageType>
int itkFiltersThresholdingProcess::updateProcess(medAbstractData* inputData)
{
    typename ImageType::Pointer inputImage = static_cast<ImageType*>(inputData->data());

    typedef itk::ThresholdImageFilter < ImageType>  ThresholdImageFilterType;
    typename ThresholdImageFilterType::Pointer thresholdFilter = ThresholdImageFilterType::New();
    thresholdFilter->SetInput(inputImage);

    if ( d->thresholdType == outsideButtonId ) // OutSide
    {
        thresholdFilter->ThresholdOutside(d->lowerValue, d->upperValue);
    }
    else if ( d->thresholdType == lowerButtonId ) // Below
    {
        thresholdFilter->ThresholdBelow(d->thresholdValue);
    }
    else if ( d->thresholdType == upperButtonId ) // Above
    {
        thresholdFilter->ThresholdAbove(d->thresholdValue);
    }
    thresholdFilter->SetOutsideValue( d->outsideValue );

    itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
    callback->SetClientData ( ( void * ) this );
    callback->SetCallback ( itkFiltersProcessBase::eventCallback );
    thresholdFilter->AddObserver ( itk::ProgressEvent(), callback );

    thresholdFilter->Update();

    getOutputData()->setData ( thresholdFilter->GetOutput() );

    QString newSeriesDescription = "threshold " + QString::number(d->thresholdValue);
    medUtilities::setDerivedMetaData(getOutputData(), inputData, newSeriesDescription);
    return DTK_SUCCEED;
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersThresholdingProcess ( void )
{
    return new itkFiltersThresholdingProcess;
}
