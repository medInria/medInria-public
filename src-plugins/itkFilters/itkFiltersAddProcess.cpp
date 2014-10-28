/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersAddProcess.h>

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <medAbstractDataFactory.h>

#include <medMetaDataKeys.h>
#include <medDoubleParameter.h>

#include <itkAddImageFilter.h>

class itkFiltersAddProcessPrivate
{
public:
    itkFiltersAddProcessPrivate(itkFiltersAddProcess *q){ parent = q; }
    virtual ~itkFiltersAddProcessPrivate(void) {}

    itkFiltersAddProcess *parent;
    medDoubleParameter *addValueParam;
    QList<medAbstractParameter*> parameters;

    template <class PixelType> void update ( void )
    {
        typedef itk::Image< PixelType, 3 > ImageType;
        typedef itk::AddImageFilter<ImageType, itk::Image<double, ImageType::ImageDimension>, ImageType> AddFilterType;
        typename AddFilterType::Pointer addFilter = AddFilterType::New();

        addFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( parent->inputImage()->data() ) ) );
        addFilter->SetConstant ( addValueParam->value() );

        itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) parent );
        parent->setCallBack(callback);

        addFilter->AddObserver ( itk::ProgressEvent(), callback );

        addFilter->Update();
        parent->output()->setData ( addFilter->GetOutput() );

        QString newSeriesDescription = parent->inputImage()->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " add filter (" + QString::number(addValueParam->value()) + ")";

        parent->output()->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    }
};

//-------------------------------------------------------------------------------------------

itkFiltersAddProcess::itkFiltersAddProcess(itkFiltersAddProcess *parent) 
    : itkFiltersProcessBase(parent), d(new itkFiltersAddProcessPrivate(this))
{    
    this->setFilter(this);
    
    this->setDescription(tr("ITK add constant filter"));

    d->addValueParam = new medDoubleParameter("Add constant value", this);
    d->addValueParam->setRange(0,1000000000);
    d->addValueParam->setValue(100);

    d->parameters << d->addValueParam;
}

//-------------------------------------------------------------------------------------------

itkFiltersAddProcess::~itkFiltersAddProcess( void )
{
}

//-------------------------------------------------------------------------------------------

bool itkFiltersAddProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkAddProcess", createitkFiltersAddProcess);
}

//-------------------------------------------------------------------------------------------

void itkFiltersAddProcess::setInputImage ( medAbstractData *data )
{
    itkFiltersProcessBase::setInputImage(data);

    if (!data)
        return;
    else
    {       
        QString identifier = data->identifier();

        if ( identifier == "itkDataImageChar3" )
        {
            d->addValueParam->setRange ( 0, std::numeric_limits<char>::max() );
        }
        else if ( identifier == "itkDataImageUChar3" )
        {
            d->addValueParam->setRange ( 0, std::numeric_limits<unsigned char>::max() );
        }
        else if ( identifier == "itkDataImageShort3" )
        {
            d->addValueParam->setRange ( 0, std::numeric_limits<short>::max() );
        }
        else if ( identifier == "itkDataImageUShort3" )
        {
            d->addValueParam->setRange ( 0, std::numeric_limits<unsigned short>::max() );
        }
        else if ( identifier == "itkDataImageInt3" )
        {
            d->addValueParam->setRange ( 0, std::numeric_limits<int>::max() );
        }
        else if ( identifier == "itkDataImageUInt3" )
        {
            d->addValueParam->setRange ( 0, std::numeric_limits<unsigned int>::max() );
        }
        else if ( identifier == "itkDataImageLong3" )
        {
            d->addValueParam->setRange ( 0, std::numeric_limits<long>::max() );
        }
        else if ( identifier== "itkDataImageULong3" )
        {
            d->addValueParam->setRange ( 0, std::numeric_limits<unsigned long>::max() );
        }
        else if ( identifier == "itkDataImageFloat3" )
        {
            d->addValueParam->setRange ( 0, std::numeric_limits<float>::max() );
        }
        else if ( identifier == "itkDataImageDouble3" )
        {
            d->addValueParam->setRange ( 0, std::numeric_limits<double>::max() );
        }
        else
        {
            qWarning() << "Error : pixel type not yet implemented ("
            << identifier
            << ")";
        }
    }
}

//-------------------------------------------------------------------------------------------

QList<medAbstractParameter*> itkFiltersAddProcess::parameters()
{
    return d->parameters;
}

//-------------------------------------------------------------------------------------------

int itkFiltersAddProcess::update ( void )
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

dtkAbstractProcess * createitkFiltersAddProcess ( void )
{
    return new itkFiltersAddProcess;
}
