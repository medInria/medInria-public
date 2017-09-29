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

#include <medUtilities.h>

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
        QString id = getInputData()->identifier();

        if ( id == "itkDataImageChar3" )
        {
            res = updateProcess<char>();
        }
        else if ( id == "itkDataImageUChar3" )
        {
            res = updateProcess<unsigned char>();
        }
        else if ( id == "itkDataImageShort3" )
        {
            res = updateProcess<short>();
        }
        else if ( id == "itkDataImageUShort3" )
        {
            res = updateProcess<unsigned short>();
        }
        else if ( id == "itkDataImageInt3" )
        {
            res = updateProcess<int>();
        }
        else if ( id == "itkDataImageUInt3" )
        {
            res = updateProcess<unsigned int>();
        }
        else if ( id == "itkDataImageLong3" )
        {
            res = updateProcess<long>();
        }
        else if ( id== "itkDataImageULong3" )
        {
            res = updateProcess<unsigned long>();
        }
        else if ( id == "itkDataImageFloat3" )
        {
            res = updateProcess<float>();
        }
        else if ( id == "itkDataImageDouble3" )
        {
            res = updateProcess<double>();
        }
        else
        {
            res = medAbstractProcess::PIXEL_TYPE;
        }
    }

    return res;
}

template <class PixelType> int itkFiltersThresholdingProcess::updateProcess()
{
    typedef itk::Image< PixelType, 3 > ImageType;
    typedef itk::ThresholdImageFilter < ImageType>  ThresholdImageFilterType;
    typename ThresholdImageFilterType::Pointer thresholdFilter = ThresholdImageFilterType::New();
    thresholdFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( getInputData()->data() ) ) );
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
    medUtilities::setDerivedMetaData(getOutputData(), getInputData(), newSeriesDescription);

    return DTK_SUCCEED;
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersThresholdingProcess ( void )
{
    return new itkFiltersThresholdingProcess;
}
