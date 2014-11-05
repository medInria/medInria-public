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
#include <medAbstractDataFactory.h>

#include <medMetaDataKeys.h>
#include <medDoubleParameter.h>

#include <itkImage.h>
#include <itkCommand.h>
#include <itkSmoothingRecursiveGaussianImageFilter.h>

#include <itkExceptionObject.h>
#include <itkSmoothingRecursiveGaussianImageFilter.h>


class itkFiltersGaussianProcessPrivate
{
public:
    itkFiltersGaussianProcessPrivate(itkFiltersGaussianProcess *q){parent = q;}
    virtual ~itkFiltersGaussianProcessPrivate(void) {}

    itkFiltersGaussianProcess *parent;
    medDoubleParameter *sigmaParam;
    QList<medAbstractParameter*> parameters;

    template <class PixelType> void update ( void )
    {
        typedef itk::Image< PixelType, 3 > ImageType;
        typedef itk::SmoothingRecursiveGaussianImageFilter< ImageType, ImageType >  GaussianFilterType;
        typename GaussianFilterType::Pointer gaussianFilter = GaussianFilterType::New();

        gaussianFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( parent->input<medAbstractData>(0)->data() ) ) );
        gaussianFilter->SetSigma( sigmaParam->value() );

        itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) parent );
        parent->setCallBack(callback);

        gaussianFilter->AddObserver ( itk::ProgressEvent(), callback );

        gaussianFilter->Update();
        parent->output<medAbstractData>(0)->setData ( gaussianFilter->GetOutput() );

        QString newSeriesDescription = parent->input<medAbstractData>(0)->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " gaussian filter (" + QString::number(sigmaParam->value()) + ")";

        parent->output<medAbstractData>(0)->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    }
};


itkFiltersGaussianProcess::itkFiltersGaussianProcess(itkFiltersGaussianProcess *parent) 
    : itkFiltersProcessBase(parent), d(new itkFiltersGaussianProcessPrivate(this))
{    
    this->setFilter(this);
    
    this->setDescription(tr("ITK gaussian filter"));

    d->sigmaParam = new medDoubleParameter("Sigma value", this);
    d->sigmaParam->setRange(0,10);
    d->sigmaParam->setValue(1.0);

    d->parameters << d->sigmaParam;
}

itkFiltersGaussianProcess::~itkFiltersGaussianProcess( void )
{
}

//-------------------------------------------------------------------------------------------

bool itkFiltersGaussianProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkGaussianProcess", createitkFiltersGaussianProcess);
}

//-------------------------------------------------------------------------------------------

QList<medAbstractParameter*> itkFiltersGaussianProcess::parameters()
{
    return d->parameters;
}

//-------------------------------------------------------------------------------------------

int itkFiltersGaussianProcess::update ( void )
{    
    if ( !this->input<medAbstractData *>(0) )
        return -1;

    QString id = this->input<medAbstractData>(0)->identifier();

    qDebug() << "itkFilters, update : " << id;

    try
    {
        if ( id == "itkDataImageChar3" )
        {
            d->update<char>();
        }
        else if ( id == "itkDataImageUChar3" )
        {
            d->update<unsigned char>();
        }
        else if ( id == "itkDataImageShort3" )
        {
            d->update<short>();
        }
        else if ( id == "itkDataImageUShort3" )
        {
            d->update<unsigned short>();
        }
        else if ( id == "itkDataImageInt3" )
        {
            d->update<int>();
        }
        else if ( id == "itkDataImageUInt3" )
        {
            d->update<unsigned int>();
        }
        else if ( id == "itkDataImageLong3" )
        {
            d->update<long>();
        }
        else if ( id== "itkDataImageULong3" )
        {
            d->update<unsigned long>();
        }
        else if ( id == "itkDataImageFloat3" )
        {
            d->update<float>();
        }
        else if ( id == "itkDataImageDouble3" )
        {
            d->update<double>();
        }
        else
        {
            qDebug() << "Error : pixel type not yet implemented ("
            << id
            << ")";
            return -1;
        }
    }
    catch (itk::ExceptionObject &e)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersGaussianProcess ( void )
{
    return new itkFiltersGaussianProcess;
}
