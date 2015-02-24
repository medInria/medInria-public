/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkFiltersGaussianProcess.h>

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <medAbstractDataFactory.h>

#include <medMetaDataKeys.h>
#include <medDoubleParameter.h>

#include <itkImage.h>
#include <itkCommand.h>
#include <itkSmoothingRecursiveGaussianImageFilter.h>

#include <itkExceptionObject.h>
#include <itkSmoothingRecursiveGaussianImageFilter.h>


class medItkFiltersGaussianProcessPrivate
{
public:
    medItkFiltersGaussianProcessPrivate(medItkFiltersGaussianProcess *q){parent = q;}
    virtual ~medItkFiltersGaussianProcessPrivate(void) {}

    medItkFiltersGaussianProcess *parent;
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


medItkFiltersGaussianProcess::medItkFiltersGaussianProcess(medItkFiltersGaussianProcess *parent)
    : medItkFiltersProcessBase(parent), d(new medItkFiltersGaussianProcessPrivate(this))
{
    this->setFilter(this);

    this->setDescription(tr("ITK gaussian filter"));

    d->sigmaParam = new medDoubleParameter("Sigma value", this);
    d->sigmaParam->setRange(0,10);
    d->sigmaParam->setValue(1.0);

    d->parameters << d->sigmaParam;
}

medItkFiltersGaussianProcess::~medItkFiltersGaussianProcess( void )
{
}

//-------------------------------------------------------------------------------------------

bool medItkFiltersGaussianProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkGaussianProcess", createmedItkFiltersGaussianProcess);
}

//-------------------------------------------------------------------------------------------

QList<medAbstractParameter*> medItkFiltersGaussianProcess::parameters()
{
    return d->parameters;
}

//-------------------------------------------------------------------------------------------

int medItkFiltersGaussianProcess::update ( void )
{
    if ( !this->input<medAbstractData>(0) )
        return -1;

    QString id = this->input<medAbstractData>(0)->identifier();

    qDebug() << "itkFilters, update : " << id;

    try
    {
        if ( id == "medItkChar3ImageData" )
        {
            d->update<char>();
        }
        else if ( id == "medItkUChar3ImageData" )
        {
            d->update<unsigned char>();
        }
        else if ( id == "medItkShort3ImageData" )
        {
            d->update<short>();
        }
        else if ( id == "medItkUShort3ImageData" )
        {
            d->update<unsigned short>();
        }
        else if ( id == "medItkInt3ImageData" )
        {
            d->update<int>();
        }
        else if ( id == "medItkUInt3ImageData" )
        {
            d->update<unsigned int>();
        }
        else if ( id == "medItkLong3ImageData" )
        {
            d->update<long>();
        }
        else if ( id== "medItkULong3ImageData" )
        {
            d->update<unsigned long>();
        }
        else if ( id == "medItkFloat3ImageData" )
        {
            d->update<float>();
        }
        else if ( id == "medItkDouble3ImageData" )
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

dtkAbstractProcess * createmedItkFiltersGaussianProcess ( void )
{
    return new medItkFiltersGaussianProcess;
}
