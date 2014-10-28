/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersWindowingProcess.h>


#include <dtkCore/dtkAbstractProcessFactory.h>
#include <medAbstractDataFactory.h>

#include <medMetaDataKeys.h>
#include <medDoubleParameter.h>

#include <itkIntensityWindowingImageFilter.h>


class itkFiltersWindowingProcessPrivate
{
public:
    itkFiltersWindowingProcessPrivate(itkFiltersWindowingProcess *q) {parent=q;}
    virtual ~itkFiltersWindowingProcessPrivate(void) {}

    itkFiltersWindowingProcess* parent;
    medDoubleParameter * minimumIntensityParam;
    medDoubleParameter * maximumIntensityParam;
    medDoubleParameter * minimumOutputIntensityParam;
    medDoubleParameter * maximumOutputIntensityParam;
    QList<medAbstractParameter*> parameters;

    template <class PixelType> void update ( void )
    {
        typedef itk::Image< PixelType, 3 > ImageType;
        typedef itk::IntensityWindowingImageFilter< ImageType, ImageType >  WindowingFilterType;
        typename WindowingFilterType::Pointer windowingFilter = WindowingFilterType::New();

        windowingFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( parent->inputImage()->data() ) ) );
        windowingFilter->SetWindowMinimum ( ( PixelType ) minimumIntensityParam->value() );
        windowingFilter->SetWindowMaximum ( ( PixelType ) maximumIntensityParam->value() );
        windowingFilter->SetOutputMinimum ( ( PixelType ) minimumOutputIntensityParam->value() );
        windowingFilter->SetOutputMaximum ( ( PixelType ) maximumOutputIntensityParam->value() );

        itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) parent );
        parent->setCallBack(callback);

        windowingFilter->AddObserver ( itk::ProgressEvent(), callback );

        windowingFilter->Update();
        parent->output()->setData ( windowingFilter->GetOutput() );

        //Set output description metadata
        QString newSeriesDescription = parent->inputImage()->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " intensity filter";

        parent->output()->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    }

};

//-------------------------------------------------------------------------------------------

itkFiltersWindowingProcess::itkFiltersWindowingProcess(itkFiltersWindowingProcess *parent) 
    : itkFiltersProcessBase(parent), d(new itkFiltersWindowingProcessPrivate(this))
{    
    this->setFilter(this);
    
    this->setDescription(tr("ITK intensity windowing filter"));

    d->minimumIntensityParam = new medDoubleParameter("Minimum", this);
    d->minimumIntensityParam->setRange(0,255);
    d->minimumIntensityParam->setValue(0);

    d->maximumIntensityParam = new medDoubleParameter("Maximum", this);
    d->maximumIntensityParam->setRange(0,255);
    d->maximumIntensityParam->setValue(255);

    d->minimumOutputIntensityParam = new medDoubleParameter("Output Minimum", this);
    d->minimumOutputIntensityParam->setRange(0,255);
    d->minimumOutputIntensityParam->setValue(0);

    d->maximumOutputIntensityParam = new medDoubleParameter("Output Maximum", this);
    d->maximumOutputIntensityParam->setRange(0,255);
    d->maximumOutputIntensityParam->setValue(255);

    d->parameters << d->minimumIntensityParam;
    d->parameters << d->maximumIntensityParam;
    d->parameters << d->minimumOutputIntensityParam;
    d->parameters << d->maximumOutputIntensityParam;
}

//-------------------------------------------------------------------------------------------

itkFiltersWindowingProcess::~itkFiltersWindowingProcess( void )
{
}

//-------------------------------------------------------------------------------------------

bool itkFiltersWindowingProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkWindowingProcess", createitkFiltersWindowingProcess);
}

//-------------------------------------------------------------------------------------------

QList<medAbstractParameter*> itkFiltersWindowingProcess::parameters()
{
    return d->parameters;
}

void itkFiltersWindowingProcess::setInputImage ( medAbstractData *data )
{
    itkFiltersProcessBase::setInputImage(data);

    if (!data)
        return;
    else
    {
        QString identifier = data->identifier();

        if ( identifier == "itkDataImageChar3" )
        {
            d->minimumIntensityParam->setRange ( std::numeric_limits<char>::min(), std::numeric_limits<char>::max() );
            d->minimumIntensityParam->setValue ( std::numeric_limits<char>::min() );

            d->maximumIntensityParam->setRange ( std::numeric_limits<char>::min(), std::numeric_limits<char>::max() );
            d->maximumIntensityParam->setValue ( std::numeric_limits<char>::max() );

            d->minimumOutputIntensityParam->setRange ( std::numeric_limits<char>::min(), std::numeric_limits<char>::max() );
            d->minimumOutputIntensityParam->setValue ( std::numeric_limits<char>::min() );

            d->maximumOutputIntensityParam->setRange ( std::numeric_limits<char>::min(), std::numeric_limits<char>::max() );
            d->maximumOutputIntensityParam->setValue ( std::numeric_limits<char>::max() );
        }
        else if ( identifier == "itkDataImageUChar3" )
        {
            d->minimumIntensityParam->setRange ( std::numeric_limits<unsigned char>::min(), std::numeric_limits<unsigned char>::max() );
            d->minimumIntensityParam->setValue ( std::numeric_limits<unsigned char>::min() );

            d->maximumIntensityParam->setRange ( std::numeric_limits<unsigned char>::min(), std::numeric_limits<unsigned char>::max() );
            d->maximumIntensityParam->setValue ( std::numeric_limits<unsigned char>::max() );

            d->minimumOutputIntensityParam->setRange ( std::numeric_limits<unsigned char>::min(), std::numeric_limits<unsigned char>::max() );
            d->minimumOutputIntensityParam->setValue ( std::numeric_limits<unsigned char>::min() );

            d->maximumOutputIntensityParam->setRange ( std::numeric_limits<unsigned char>::min(), std::numeric_limits<unsigned char>::max() );
            d->maximumOutputIntensityParam->setValue ( std::numeric_limits<unsigned char>::max() );
        }
        else if ( identifier == "itkDataImageShort3" )
        {
            d->minimumIntensityParam->setRange ( std::numeric_limits<short>::min(), std::numeric_limits<short>::max() );
            d->minimumIntensityParam->setValue ( std::numeric_limits<short>::min() );

            d->maximumIntensityParam->setRange ( std::numeric_limits<short>::min(), std::numeric_limits<short>::max() );
            d->maximumIntensityParam->setValue ( std::numeric_limits<short>::max() );

            d->minimumOutputIntensityParam->setRange ( std::numeric_limits<short>::min(), std::numeric_limits<short>::max() );
            d->minimumOutputIntensityParam->setValue ( std::numeric_limits<short>::min() );

            d->maximumOutputIntensityParam->setRange ( std::numeric_limits<short>::min(), std::numeric_limits<short>::max() );
            d->maximumOutputIntensityParam->setValue ( std::numeric_limits<short>::max() );
        }
        else if ( identifier == "itkDataImageUShort3" )
        {
            d->minimumIntensityParam->setRange ( std::numeric_limits<unsigned short>::min(), std::numeric_limits<unsigned short>::max() );
            d->minimumIntensityParam->setValue ( std::numeric_limits<unsigned short>::min() );

            d->maximumIntensityParam->setRange ( std::numeric_limits<unsigned short>::min(), std::numeric_limits<unsigned short>::max() );
            d->maximumIntensityParam->setValue ( std::numeric_limits<unsigned short>::max() );

            d->minimumOutputIntensityParam->setRange ( std::numeric_limits<unsigned short>::min(), std::numeric_limits<unsigned short>::max() );
            d->minimumOutputIntensityParam->setValue ( std::numeric_limits<unsigned short>::min() );

            d->maximumOutputIntensityParam->setRange ( std::numeric_limits<unsigned short>::min(), std::numeric_limits<unsigned short>::max() );
            d->maximumOutputIntensityParam->setValue ( std::numeric_limits<unsigned short>::max() );
        }
        else if ( identifier == "itkDataImageInt3" )
        {
            d->minimumIntensityParam->setRange ( std::numeric_limits<int>::min(), std::numeric_limits<int>::max() );
            d->minimumIntensityParam->setValue ( std::numeric_limits<int>::min() );

            d->maximumIntensityParam->setRange ( std::numeric_limits<int>::min(), std::numeric_limits<int>::max() );
            d->maximumIntensityParam->setValue ( std::numeric_limits<int>::max() );

            d->minimumOutputIntensityParam->setRange ( std::numeric_limits<int>::min(), std::numeric_limits<int>::max() );
            d->minimumOutputIntensityParam->setValue ( std::numeric_limits<int>::min() );

            d->maximumOutputIntensityParam->setRange ( std::numeric_limits<int>::min(), std::numeric_limits<int>::max() );
            d->maximumOutputIntensityParam->setValue ( std::numeric_limits<int>::max() );
        }
        else if ( identifier == "itkDataImageUInt3" )
        {
            d->minimumIntensityParam->setRange ( std::numeric_limits<unsigned int>::min(), std::numeric_limits<unsigned int>::max() );
            d->minimumIntensityParam->setValue ( std::numeric_limits<unsigned int>::min() );

            d->maximumIntensityParam->setRange ( std::numeric_limits<unsigned int>::min(), std::numeric_limits<unsigned int>::max() );
            d->maximumIntensityParam->setValue ( std::numeric_limits<unsigned int>::max() );

            d->minimumOutputIntensityParam->setRange ( std::numeric_limits<unsigned int>::min(), std::numeric_limits<unsigned int>::max() );
            d->minimumOutputIntensityParam->setValue ( std::numeric_limits<unsigned int>::min() );

            d->maximumOutputIntensityParam->setRange ( std::numeric_limits<unsigned int>::min(), std::numeric_limits<unsigned int>::max() );
            d->maximumOutputIntensityParam->setValue ( std::numeric_limits<unsigned int>::max() );
        }
        else if ( identifier == "itkDataImageLong3" )
        {
            d->minimumIntensityParam->setRange ( std::numeric_limits<long>::min(), std::numeric_limits<long>::max() );
            d->minimumIntensityParam->setValue ( std::numeric_limits<long>::min() );

            d->maximumIntensityParam->setRange ( std::numeric_limits<long>::min(), std::numeric_limits<long>::max() );
            d->maximumIntensityParam->setValue ( std::numeric_limits<long>::max() );

            d->minimumOutputIntensityParam->setRange ( std::numeric_limits<long>::min(), std::numeric_limits<long>::max() );
            d->minimumOutputIntensityParam->setValue ( std::numeric_limits<long>::min() );

            d->maximumOutputIntensityParam->setRange ( std::numeric_limits<long>::min(), std::numeric_limits<long>::max() );
            d->maximumOutputIntensityParam->setValue ( std::numeric_limits<long>::max() );
        }
        else if ( identifier== "itkDataImageULong3" )
        {
            d->minimumIntensityParam->setRange ( std::numeric_limits<unsigned long>::min(), std::numeric_limits<unsigned long>::max() );
            d->minimumIntensityParam->setValue ( std::numeric_limits<unsigned long>::min() );

            d->maximumIntensityParam->setRange ( std::numeric_limits<unsigned long>::min(), std::numeric_limits<unsigned long>::max() );
            d->maximumIntensityParam->setValue ( std::numeric_limits<unsigned long>::max() );

            d->minimumOutputIntensityParam->setRange ( std::numeric_limits<unsigned long>::min(), std::numeric_limits<unsigned long>::max() );
            d->minimumOutputIntensityParam->setValue ( std::numeric_limits<unsigned long>::min() );

            d->maximumOutputIntensityParam->setRange ( std::numeric_limits<unsigned long>::min(), std::numeric_limits<unsigned long>::max() );
            d->maximumOutputIntensityParam->setValue ( std::numeric_limits<unsigned long>::max() );
        }
        else if ( identifier == "itkDataImageFloat3" )
        {
            d->minimumIntensityParam->setRange ( std::numeric_limits<float>::min(), std::numeric_limits<float>::max() );
            d->minimumIntensityParam->setValue ( std::numeric_limits<float>::min() );

            d->maximumIntensityParam->setRange ( std::numeric_limits<float>::min(), std::numeric_limits<float>::max() );
            d->maximumIntensityParam->setValue ( std::numeric_limits<float>::max() );

            d->minimumOutputIntensityParam->setRange ( std::numeric_limits<float>::min(), std::numeric_limits<float>::max() );
            d->minimumOutputIntensityParam->setValue ( std::numeric_limits<float>::min() );

            d->maximumOutputIntensityParam->setRange ( std::numeric_limits<float>::min(), std::numeric_limits<float>::max() );
            d->maximumOutputIntensityParam->setValue ( std::numeric_limits<float>::max() );
        }
        else if ( identifier == "itkDataImageDouble3" )
        {
            d->minimumIntensityParam->setRange ( std::numeric_limits<double>::min(), std::numeric_limits<double>::max() );
            d->minimumIntensityParam->setValue ( std::numeric_limits<double>::min() );

            d->maximumIntensityParam->setRange ( std::numeric_limits<double>::min(), std::numeric_limits<double>::max() );
            d->maximumIntensityParam->setValue ( std::numeric_limits<double>::max() );

            d->minimumOutputIntensityParam->setRange ( std::numeric_limits<double>::min(), std::numeric_limits<double>::max() );
            d->minimumOutputIntensityParam->setValue ( std::numeric_limits<double>::min() );

            d->maximumOutputIntensityParam->setRange ( std::numeric_limits<double>::min(), std::numeric_limits<double>::max() );
            d->maximumOutputIntensityParam->setValue ( std::numeric_limits<double>::max() );
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

int itkFiltersWindowingProcess::update ( void )
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

dtkAbstractProcess * createitkFiltersWindowingProcess ( void )
{
    return new itkFiltersWindowingProcess;
}
