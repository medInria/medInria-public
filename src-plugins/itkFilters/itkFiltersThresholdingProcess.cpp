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
    double threshold;
    int outsideValue;
    bool comparisonOperator;
};

const double itkFiltersThresholdingProcess::defaultThreshold = 200.0;
const int itkFiltersThresholdingProcess::defaultOutsideValue = 0;
const bool itkFiltersThresholdingProcess::defaultComparisonOperator = true;

itkFiltersThresholdingProcess::itkFiltersThresholdingProcess(itkFiltersThresholdingProcess *parent) 
    : itkFiltersProcessBase(parent), d(new itkFiltersThresholdingProcessPrivate)
{   
    d->threshold = defaultThreshold;
    d->outsideValue = defaultOutsideValue;
    d->comparisonOperator = defaultComparisonOperator;
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

void itkFiltersThresholdingProcess::setParameter(double data, int channel)
{
    if (channel > 2)
        return;

    if (channel == 0)
        d->threshold = data;
    if (channel == 1)
        d->outsideValue = data;
    if (channel == 2)
        d->comparisonOperator = (bool)data;
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
    if (d->comparisonOperator)
        thresholdFilter->SetUpper( d->threshold ); // <= threshold
    else
        thresholdFilter->SetLower( d->threshold );
    thresholdFilter->SetOutsideValue( d->outsideValue );

    itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
    callback->SetClientData ( ( void * ) this );
    callback->SetCallback ( itkFiltersProcessBase::eventCallback );
    thresholdFilter->AddObserver ( itk::ProgressEvent(), callback );

    thresholdFilter->Update();

    getOutputData()->setData ( thresholdFilter->GetOutput() );

    QString newSeriesDescription = "threshold " + QString::number(d->threshold);
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
