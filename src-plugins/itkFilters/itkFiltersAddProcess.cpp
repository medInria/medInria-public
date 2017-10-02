/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkFiltersAddProcess.h"

#include <dtkCore/dtkAbstractProcessFactory.h>

#include <itkAddImageFilter.h>
#include <itkImage.h>

#include <medUtilities.h>

class itkFiltersAddProcessPrivate
{
public:
    double addValue;
};

const double itkFiltersAddProcess::defaultAddValue = 100.0;

//-------------------------------------------------------------------------------------------

itkFiltersAddProcess::itkFiltersAddProcess(itkFiltersAddProcess *parent) 
    : itkFiltersProcessBase(parent), d(new itkFiltersAddProcessPrivate)
{  
    d->addValue = defaultAddValue;
}

itkFiltersAddProcess::itkFiltersAddProcess(const itkFiltersAddProcess& other)
     : itkFiltersProcessBase(other), d(other.d)
{
}

//-------------------------------------------------------------------------------------------

itkFiltersAddProcess::~itkFiltersAddProcess( void )
{
    delete d;
}

//-------------------------------------------------------------------------------------------

bool itkFiltersAddProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkAddProcess", createitkFiltersAddProcess);
}

//-------------------------------------------------------------------------------------------

QString itkFiltersAddProcess::description() const
{
    return tr("ITK add constant filter");
}

//-------------------------------------------------------------------------------------------

void itkFiltersAddProcess::setParameter(double data, int channel)
{
    if (channel != 0)
        return;
    
    d->addValue = data;
}

//-------------------------------------------------------------------------------------------

int itkFiltersAddProcess::tryUpdate()
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

template <class PixelType> int itkFiltersAddProcess::updateProcess()
{
    typedef itk::Image< PixelType, 3 > ImageType;
    typedef itk::AddImageFilter<ImageType, itk::Image<double, ImageType::ImageDimension>, ImageType> AddFilterType;
    typename AddFilterType::Pointer addFilter = AddFilterType::New();

    addFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( getInputData()->data() ) ) );
    addFilter->SetConstant ( d->addValue );

    itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
    callback->SetClientData ( ( void * ) this );
    callback->SetCallback ( itkFiltersProcessBase::eventCallback );
    addFilter->AddObserver ( itk::ProgressEvent(), callback );

    addFilter->Update();

    getOutputData()->setData ( addFilter->GetOutput() );

    QString newSeriesDescription = "add filter " + QString::number(d->addValue);
    medUtilities::setDerivedMetaData(getOutputData(), getInputData(), newSeriesDescription);

    return DTK_SUCCEED;
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersAddProcess ( void )
{
    return new itkFiltersAddProcess;
}
