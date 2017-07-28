/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersGaussianProcess.h>

#include <dtkCore/dtkAbstractProcessFactory.h>

#include <itkImage.h>
#include <itkSmoothingRecursiveGaussianImageFilter.h>

#include <medUtilities.h>

class itkFiltersGaussianProcessPrivate
{
public:
    double sigma;
};

itkFiltersGaussianProcess::itkFiltersGaussianProcess(itkFiltersGaussianProcess *parent) 
    : itkFiltersProcessBase(), d(new itkFiltersGaussianProcessPrivate)
{
    filter = this;
    descriptionText = tr("ITK gaussian filter");

    d->sigma = 1;
}

//-------------------------------------------------------------------------------------------

itkFiltersGaussianProcess::~itkFiltersGaussianProcess( void )
{
    delete d;
}

//-------------------------------------------------------------------------------------------

bool itkFiltersGaussianProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkGaussianProcess", createitkFiltersGaussianProcess);
}

//-------------------------------------------------------------------------------------------

void itkFiltersGaussianProcess::setParameter(double data, int channel)
{
    if (channel != 0)
        return;
    
    d->sigma = data;
}

//-------------------------------------------------------------------------------------------

int itkFiltersGaussianProcess::tryUpdate()
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

template <class PixelType> int itkFiltersGaussianProcess::updateProcess()
{
    typedef itk::Image< PixelType, 3 > ImageType;
    typedef itk::SmoothingRecursiveGaussianImageFilter< ImageType, ImageType >  GaussianFilterType;
    typename GaussianFilterType::Pointer gaussianFilter = GaussianFilterType::New();

    gaussianFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( inputData->data() ) ) );
    gaussianFilter->SetSigma( d->sigma );

    callback = itk::CStyleCommand::New();
    callback->SetClientData ( ( void * ) this );
    callback->SetCallback ( itkFiltersProcessBase::eventCallback );
    gaussianFilter->AddObserver ( itk::ProgressEvent(), callback );

    gaussianFilter->Update();

    outputData->setData ( gaussianFilter->GetOutput() );

    QString newSeriesDescription = "gaussian filter " + QString::number(d->sigma);
    medUtilities::setDerivedMetaData(outputData, inputData, newSeriesDescription);

    return DTK_SUCCEED;
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersGaussianProcess ( void )
{
    return new itkFiltersGaussianProcess;
}
