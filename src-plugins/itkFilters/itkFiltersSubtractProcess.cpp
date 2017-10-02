/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersSubtractProcess.h>

#include <dtkCore/dtkAbstractProcessFactory.h>

#include <itkImage.h>
#include <itkSubtractImageFilter.h>

#include <medUtilities.h>

class itkFiltersSubtractProcessPrivate
{
public:
    double subtractValue;
};

const double itkFiltersSubtractProcess::defaultSubtractValue = 100.0;

//-------------------------------------------------------------------------------------------

itkFiltersSubtractProcess::itkFiltersSubtractProcess(itkFiltersSubtractProcess *parent) 
    : itkFiltersProcessBase(parent), d(new itkFiltersSubtractProcessPrivate)
{   
    d->subtractValue = defaultSubtractValue;
}

itkFiltersSubtractProcess::itkFiltersSubtractProcess(const itkFiltersSubtractProcess& other)
     : itkFiltersProcessBase(other), d(other.d)
{
}

//-------------------------------------------------------------------------------------------

itkFiltersSubtractProcess::~itkFiltersSubtractProcess( void )
{
    delete d;
}

//-------------------------------------------------------------------------------------------

bool itkFiltersSubtractProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkSubtractProcess", createitkFiltersSubtractProcess);
}

//-------------------------------------------------------------------------------------------

QString itkFiltersSubtractProcess::description() const
{
    return tr("ITK subtract constant filter");
}

//-------------------------------------------------------------------------------------------

void itkFiltersSubtractProcess::setParameter(double data, int channel)
{
    if (channel != 0)
        return;
    
    d->subtractValue = data;
}

//-------------------------------------------------------------------------------------------

int itkFiltersSubtractProcess::tryUpdate()
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

template <class PixelType> int itkFiltersSubtractProcess::updateProcess()
{
    typedef itk::Image< PixelType, 3 > ImageType;
    typedef itk::SubtractImageFilter< ImageType, itk::Image<double, ImageType::ImageDimension>, ImageType >  SubtractFilterType;
    typename SubtractFilterType::Pointer subtractFilter = SubtractFilterType::New();

    subtractFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( getInputData()->data() ) ) );
    subtractFilter->SetConstant ( d->subtractValue );

    itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
    callback->SetClientData ( ( void * ) this );
    callback->SetCallback ( itkFiltersProcessBase::eventCallback );
    subtractFilter->AddObserver ( itk::ProgressEvent(), callback );

    subtractFilter->Update();

    getOutputData()->setData ( subtractFilter->GetOutput() );

    //Set output description metadata
    QString newSeriesDescription = "subtract filter " + QString::number(d->subtractValue);
    medUtilities::setDerivedMetaData(getOutputData(), getInputData(), newSeriesDescription);

    return DTK_SUCCEED;
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersSubtractProcess ( void )
{
    return new itkFiltersSubtractProcess;
}


























