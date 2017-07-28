/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersMedianProcess.h>

#include <dtkCore/dtkAbstractProcessFactory.h>

#include <itkImage.h>
#include <itkMedianImageFilter.h>

#include <medUtilities.h>

//-------------------------------------------------------------------------------------------

itkFiltersMedianProcess::itkFiltersMedianProcess(itkFiltersMedianProcess *parent) 
    : itkFiltersProcessBase()
{
    filter = this;
    descriptionText = tr("ITK median filter");
}

//-------------------------------------------------------------------------------------------

bool itkFiltersMedianProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkMedianProcess", createitkFiltersMedianProcess);
}

//-------------------------------------------------------------------------------------------

int itkFiltersMedianProcess::tryUpdate()
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

template <class PixelType> int itkFiltersMedianProcess::updateProcess()
{
    typedef itk::Image< PixelType, 3 > ImageType;
    typedef itk::MedianImageFilter< ImageType, ImageType >  MedianFilterType;
    typename MedianFilterType::Pointer medianFilter = MedianFilterType::New();

    medianFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( inputData->data() ) ) );

    callback = itk::CStyleCommand::New();
    callback->SetClientData ( ( void * ) this );
    callback->SetCallback ( itkFiltersProcessBase::eventCallback );
    medianFilter->AddObserver ( itk::ProgressEvent(), callback );

    medianFilter->Update();

    outputData->setData ( medianFilter->GetOutput() );

    //Set output description metadata
    medUtilities::setDerivedMetaData(outputData, inputData, "median filter");

    return DTK_SUCCEED;
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersMedianProcess ( void )
{
    return new itkFiltersMedianProcess;
}
