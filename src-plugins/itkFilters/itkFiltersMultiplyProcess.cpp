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
#include <medAbstractDataFactory.h>

#include <medMetaDataKeys.h>
#include <medDoubleParameter.h>

#include <itkMultiplyImageFilter.h>

class itkFiltersMultiplyProcessPrivate
{
public:
    itkFiltersMultiplyProcessPrivate(itkFiltersMultiplyProcess *q) {parent = q;}
    virtual ~itkFiltersMultiplyProcessPrivate(void) {}

    itkFiltersMultiplyProcess *parent;
    medDoubleParameter *multiplyFactorParam;
    QList<medAbstractParameter*> parameters;

    template <class PixelType> void update ( void )
    {
        typedef itk::Image< PixelType, 3 > ImageType;
        typedef itk::MultiplyImageFilter< ImageType, itk::Image<double, ImageType::ImageDimension>, ImageType >  MultiplyFilterType;
        typename MultiplyFilterType::Pointer multiplyFilter = MultiplyFilterType::New();

        multiplyFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( parent->inputImage()->data() ) ) );
        multiplyFilter->SetConstant ( multiplyFactorParam->value() );

        itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) parent );
        parent->setCallBack(callback);

        multiplyFilter->AddObserver ( itk::ProgressEvent(), callback );

        multiplyFilter->Update();
        parent->output()->setData ( multiplyFilter->GetOutput() );

        //Set output description metadata
        QString newSeriesDescription = parent->inputImage()->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " multiply filter (" + QString::number(multiplyFactorParam->value()) + ")";

        parent->output()->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    }

};

//-------------------------------------------------------------------------------------------

itkFiltersMultiplyProcess::itkFiltersMultiplyProcess(itkFiltersMultiplyProcess *parent) 
    : itkFiltersProcessBase(parent), d(new itkFiltersMultiplyProcessPrivate(this))
{    
    this->setFilter(this);
    
    this->setDescription(tr("ITK multiply by constant filter"));

    d->multiplyFactorParam = new medDoubleParameter("Multiply constant value", this);
    d->multiplyFactorParam->setRange(0,1000000000);
    d->multiplyFactorParam->setValue(2.0);

    d->parameters << d->multiplyFactorParam;
}

//-------------------------------------------------------------------------------------------

itkFiltersMultiplyProcess::~itkFiltersMultiplyProcess( void )
{
}

//-------------------------------------------------------------------------------------------

bool itkFiltersMultiplyProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkMultiplyProcess", createitkFiltersMultiplyProcess);
}

//-------------------------------------------------------------------------------------------

QList<medAbstractParameter*> itkFiltersMultiplyProcess::parameters()
{
    return d->parameters;
}

//-------------------------------------------------------------------------------------------

int itkFiltersMultiplyProcess::update ( void )
{
    if ( !this->inputImage() )
        return -1;

    QString id = this->inputImage()->identifier();

    qDebug() << "itkFilters, update : " << id;

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

    return EXIT_SUCCESS;
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersMultiplyProcess ( void )
{
    return new itkFiltersMultiplyProcess;
}
