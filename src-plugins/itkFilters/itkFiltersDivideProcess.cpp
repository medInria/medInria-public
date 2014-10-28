/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersDivideProcess.h>

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <medAbstractDataFactory.h>

#include <medMetaDataKeys.h>
#include <medDoubleParameter.h>

#include <itkDivideImageFilter.h>


class itkFiltersDivideProcessPrivate
{
public:
    itkFiltersDivideProcessPrivate(itkFiltersDivideProcess *q) { parent = q; }

    virtual ~itkFiltersDivideProcessPrivate(void) {}

    itkFiltersDivideProcess *parent;
    medDoubleParameter *divideFactorParam;
    QList<medAbstractParameter*> parameters;

    template <class PixelType> void update ( void )
    {
        typedef itk::Image< PixelType, 3 > ImageType;
        typedef itk::DivideImageFilter< ImageType, itk::Image<double, ImageType::ImageDimension>, ImageType >  DivideFilterType;
        typename DivideFilterType::Pointer divideFilter = DivideFilterType::New();

        divideFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( parent->inputImage()->data() ) ) );
        divideFilter->SetConstant ( divideFactorParam->value() );

        itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) parent );
        parent->setCallBack(callback);

        divideFilter->AddObserver ( itk::ProgressEvent(), callback );

        divideFilter->Update();
        parent->output()->setData ( divideFilter->GetOutput() );

        QString newSeriesDescription = parent->inputImage()->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " divide filter (" + QString::number(divideFactorParam->value()) + ")";

        parent->output()->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    }
};

//-------------------------------------------------------------------------------------------

itkFiltersDivideProcess::itkFiltersDivideProcess(itkFiltersDivideProcess *parent) 
    : itkFiltersProcessBase(parent), d(new itkFiltersDivideProcessPrivate(this))
{    
    this->setFilter(this);

    this->setDescription(tr("ITK divide by constant filter"));

    d->divideFactorParam = new medDoubleParameter("Divide constant value", this);
    d->divideFactorParam->setRange(0,1000000000);
    d->divideFactorParam->setValue(2.0);

    d->parameters << d->divideFactorParam;
}

//-------------------------------------------------------------------------------------------

itkFiltersDivideProcess::~itkFiltersDivideProcess( void )
{
}

//-------------------------------------------------------------------------------------------

bool itkFiltersDivideProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkDivideProcess", createitkFiltersDivideProcess);
}

//-------------------------------------------------------------------------------------------

QList<medAbstractParameter*> itkFiltersDivideProcess::parameters()
{
    return d->parameters;
}


//-------------------------------------------------------------------------------------------

int itkFiltersDivideProcess::update ( void )
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

dtkAbstractProcess * createitkFiltersDivideProcess ( void )
{
    return new itkFiltersDivideProcess;
}
