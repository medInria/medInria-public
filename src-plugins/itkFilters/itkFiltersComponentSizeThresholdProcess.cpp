/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkFiltersComponentSizeThresholdProcess.h"

#include <dtkCore/dtkAbstractProcessFactory.h>

#include <itkBinaryThresholdImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkConnectedComponentImageFilter.h>
#include <itkImage.h>
#include <itkRelabelComponentImageFilter.h>

#include <medAbstractDataFactory.h>
#include <medUtilities.h>

class itkFiltersComponentSizeThresholdProcessPrivate
{
public:
    double minimumSize;
};

//-------------------------------------------------------------------------------------------

itkFiltersComponentSizeThresholdProcess::itkFiltersComponentSizeThresholdProcess(itkFiltersComponentSizeThresholdProcess *parent) 
    : itkFiltersProcessBase(), d(new itkFiltersComponentSizeThresholdProcessPrivate)
{  
    filter = this;
    descriptionText = tr("Size Threshold filter");

    d->minimumSize = 50;
}

//-------------------------------------------------------------------------------------------

itkFiltersComponentSizeThresholdProcess::~itkFiltersComponentSizeThresholdProcess( void )
{
    delete d;
}

//-------------------------------------------------------------------------------------------

bool itkFiltersComponentSizeThresholdProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkComponentSizeThresholdProcess", createitkFiltersComponentSizeThresholdProcess);
}

//-------------------------------------------------------------------------------------------

void itkFiltersComponentSizeThresholdProcess::setParameter(int data, int channel)
{
    if (channel != 0)
        return;
     
    d->minimumSize = data;
}

//-------------------------------------------------------------------------------------------

int itkFiltersComponentSizeThresholdProcess::tryUpdate()
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
        else if ( id== "itkDataImageFloat3" )
        {
            if (castToUInt3<float>() == DTK_SUCCEED)
            {
                res = updateProcess<unsigned int>();
            }
        }
        else if ( id== "itkDataImageDouble3" )
        {
            if(castToUInt3<double>() == DTK_SUCCEED)
            {
                res = updateProcess<unsigned int>();
            }
        }
        else
        {
            res = medAbstractProcess::PIXEL_TYPE;
        }
    }

    return res;
}

template <class PixelType> int itkFiltersComponentSizeThresholdProcess::castToUInt3 ( void )
{
    //we will later label the image so we don't care about precision.
    typedef itk::Image< PixelType, 3 > InputImageType;
    typedef itk::Image< unsigned int, 3 > OutputImageType;
    typedef itk::CastImageFilter< InputImageType, OutputImageType > CastFilterType;

    typename CastFilterType::Pointer  caster = CastFilterType::New();
    typename InputImageType::Pointer im = dynamic_cast< InputImageType*>((itk::Object*)(inputData->data()));
    caster->SetInput(im);
    caster->Update();

    inputData = medAbstractDataFactory::instance()->createSmartPointer ( "itkDataImageUInt3" );
    inputData->setData(caster->GetOutput());

    return DTK_SUCCEED;
}

template <class PixelType> int itkFiltersComponentSizeThresholdProcess::updateProcess()
{
    typedef itk::Image< PixelType, 3 > ImageType;
    typedef itk::Image< unsigned short, 3 > OutputImageType;

    typedef itk::ConnectedComponentImageFilter <ImageType, OutputImageType> ConnectedComponentFilterType;
    typename ConnectedComponentFilterType::Pointer connectedComponentFilter = ConnectedComponentFilterType::New();
    connectedComponentFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( inputData->data() ) ) );
    connectedComponentFilter->Update();

    // RELABEL COMPONENTS according to their sizes (0:largest(background))
    typedef itk::RelabelComponentImageFilter<OutputImageType, OutputImageType> FilterType;
    typename FilterType::Pointer relabelFilter = FilterType::New();
    relabelFilter->SetInput(connectedComponentFilter->GetOutput());
    relabelFilter->SetMinimumObjectSize(d->minimumSize);
    relabelFilter->Update();

    // BINARY FILTER
    typedef itk::BinaryThresholdImageFilter <OutputImageType, OutputImageType> BinaryThresholdImageFilterType;
    typename BinaryThresholdImageFilterType::Pointer thresholdFilter
            = BinaryThresholdImageFilterType::New();
    thresholdFilter->SetInput(relabelFilter->GetOutput());
    thresholdFilter->SetUpperThreshold(0);
    thresholdFilter->SetInsideValue(0);
    thresholdFilter->SetOutsideValue(1);

    thresholdFilter->Update();

    callback = itk::CStyleCommand::New();
    callback->SetClientData ( ( void * ) this );
    callback->SetCallback ( itkFiltersProcessBase::eventCallback );
    connectedComponentFilter->AddObserver ( itk::ProgressEvent(), callback );

    outputData = medAbstractDataFactory::instance()->createSmartPointer ( "itkDataImageUShort3" );
    outputData->setData ( thresholdFilter->GetOutput() );

    QString newSeriesDescription = "connectedComponent " + QString::number(d->minimumSize);
    medUtilities::setDerivedMetaData(outputData, inputData, newSeriesDescription);

    return DTK_SUCCEED;
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersComponentSizeThresholdProcess ( void )
{
    return new itkFiltersComponentSizeThresholdProcess;
}
