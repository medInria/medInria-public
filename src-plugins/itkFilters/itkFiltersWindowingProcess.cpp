/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersWindowingProcess.h>

#include <dtkCore/dtkAbstractProcessFactory.h>

#include <itkImage.h>
#include <itkIntensityWindowingImageFilter.h>

#include <medUtilities.h>

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

itkFiltersWindowingProcess::~itkFiltersWindowingProcess( void )
{
    delete d;
}

//-------------------------------------------------------------------------------------------

bool itkFiltersWindowingProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkWindowingProcess", createitkFiltersWindowingProcess);
}

//-------------------------------------------------------------------------------------------

QString itkFiltersWindowingProcess::description() const
{
    return tr("ITK intensity windowing filter");
}

//-------------------------------------------------------------------------------------------

void itkFiltersWindowingProcess::setParameter(double data, int channel)
{
    if (channel > 3)
        return;
    
    switch ( channel )
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
    }
}

//-------------------------------------------------------------------------------------------

int itkFiltersWindowingProcess::tryUpdate()
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

template <class PixelType> int itkFiltersWindowingProcess::updateProcess()
{
    typedef itk::Image< PixelType, 3 > ImageType;
    typedef itk::IntensityWindowingImageFilter< ImageType, ImageType >  WindowingFilterType;
    typename WindowingFilterType::Pointer windowingFilter = WindowingFilterType::New();

    windowingFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( getInputData()->data() ) ) );
    windowingFilter->SetWindowMinimum ( ( PixelType ) d->minimumIntensityValue );
    windowingFilter->SetWindowMaximum ( ( PixelType ) d->maximumIntensityValue );
    windowingFilter->SetOutputMinimum ( ( PixelType ) d->minimumOutputIntensityValue );
    windowingFilter->SetOutputMaximum ( ( PixelType ) d->maximumOutputIntensityValue );

    itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
    callback->SetClientData ( ( void * ) this );
    callback->SetCallback ( itkFiltersProcessBase::eventCallback );
    windowingFilter->AddObserver ( itk::ProgressEvent(), callback );

    windowingFilter->Update();

    getOutputData()->setData ( windowingFilter->GetOutput() );

    //Set output description metadata
    QString newSeriesDescription = "windowing " + QString::number(d->minimumIntensityValue)
            + " " + QString::number(d->maximumIntensityValue);
    medUtilities::setDerivedMetaData(getOutputData(), getInputData(), newSeriesDescription);

    return DTK_SUCCEED;
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersWindowingProcess ( void )
{
    return new itkFiltersWindowingProcess;
}
