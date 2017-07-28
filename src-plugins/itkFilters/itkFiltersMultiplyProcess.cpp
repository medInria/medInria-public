/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersMultiplyProcess.h>

#include <dtkCore/dtkAbstractProcessFactory.h>

#include <itkImage.h>
#include <itkMultiplyImageFilter.h>

#include <medUtilities.h>

class itkFiltersMultiplyProcessPrivate
{
public:
    double multiplyFactor;
};

//-------------------------------------------------------------------------------------------

itkFiltersMultiplyProcess::itkFiltersMultiplyProcess(itkFiltersMultiplyProcess *parent) 
    : itkFiltersProcessBase(), d(new itkFiltersMultiplyProcessPrivate)
{  
    filter = this;
    descriptionText = tr("ITK multiply by constant filter");

    d->multiplyFactor = 2.0;
}

//-------------------------------------------------------------------------------------------

itkFiltersMultiplyProcess::~itkFiltersMultiplyProcess( void )
{
    delete d;
}

//-------------------------------------------------------------------------------------------

bool itkFiltersMultiplyProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkMultiplyProcess", createitkFiltersMultiplyProcess);
}

//-------------------------------------------------------------------------------------------

void itkFiltersMultiplyProcess::setParameter(double data, int channel)
{
    if (channel != 0)
        return;
    
    d->multiplyFactor = data;
}

//-------------------------------------------------------------------------------------------

int itkFiltersMultiplyProcess::tryUpdate()
{  
    int res = DTK_FAILURE;

    if ( inputData )
    {
        QString id = inputData->identifier();

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

template <class PixelType> int itkFiltersMultiplyProcess::updateProcess()
{
    typedef itk::Image< PixelType, 3 > ImageType;
    typedef itk::MultiplyImageFilter< ImageType, itk::Image<double, ImageType::ImageDimension>, ImageType >  MultiplyFilterType;
    typename MultiplyFilterType::Pointer multiplyFilter = MultiplyFilterType::New();

    multiplyFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( inputData->data() ) ) );
    multiplyFilter->SetConstant ( d->multiplyFactor );

    callback = itk::CStyleCommand::New();
    callback->SetClientData ( ( void * ) this );
    callback->SetCallback ( itkFiltersProcessBase::eventCallback );
    multiplyFilter->AddObserver ( itk::ProgressEvent(), callback );

    multiplyFilter->Update();

    outputData->setData ( multiplyFilter->GetOutput() );

    //Set output description metadata
    QString newSeriesDescription = "multiply filter " + QString::number(d->multiplyFactor);
    medUtilities::setDerivedMetaData(outputData, inputData, newSeriesDescription);

    return DTK_SUCCEED;
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersMultiplyProcess ( void )
{
    return new itkFiltersMultiplyProcess;
}
