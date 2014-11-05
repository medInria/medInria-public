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
#include <medAbstractDataFactory.h>

#include <medMetaDataKeys.h>

#include <itkMedianImageFilter.h>


class itkFiltersMedianProcessPrivate
{
public:
    itkFiltersMedianProcessPrivate(itkFiltersMedianProcess *q) {parent = q;}
    virtual ~itkFiltersMedianProcessPrivate(void) {}

    itkFiltersMedianProcess* parent;

    template <class PixelType> void update ( void )
    {
        typedef itk::Image< PixelType, 3 > ImageType;
        typedef itk::MedianImageFilter< ImageType, ImageType >  MedianFilterType;
        typename MedianFilterType::Pointer medianFilter = MedianFilterType::New();

        medianFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( parent->input<medAbstractData*>(0)->data() ) ) );

        itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) parent );
        parent->setCallBack(callback);

        medianFilter->AddObserver ( itk::ProgressEvent(), callback );

        medianFilter->Update();
        parent->output<medAbstractData*>(0)->setData ( medianFilter->GetOutput() );

        //Set output description metadata
        QString newSeriesDescription = parent->input<medAbstractData*>(0)->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " median filter";

        parent->output<medAbstractData*>(0)->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    }
};


//-------------------------------------------------------------------------------------------

itkFiltersMedianProcess::itkFiltersMedianProcess(itkFiltersMedianProcess *parent) 
    : itkFiltersProcessBase(parent), d(new itkFiltersMedianProcessPrivate(this))
{    
    this->setFilter(this);
    
    this->setDescription(tr("ITK median filter"));
}

//-------------------------------------------------------------------------------------------

itkFiltersMedianProcess::~itkFiltersMedianProcess( void )
{
}

//-------------------------------------------------------------------------------------------

bool itkFiltersMedianProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkMedianProcess", createitkFiltersMedianProcess);
}

//-------------------------------------------------------------------------------------------

int itkFiltersMedianProcess::update ( void )
{    
    if ( !this->input<medAbstractData*>(0) )
        return -1;

    QString id = this->input<medAbstractData*>(0)->identifier();

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

dtkAbstractProcess * createitkFiltersMedianProcess ( void )
{
    return new itkFiltersMedianProcess;
}
