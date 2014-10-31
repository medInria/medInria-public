/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersInvertProcess.h>

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <medAbstractDataFactory.h>

#include <medMetaDataKeys.h>

#include <itkInvertIntensityImageFilter.h>

class itkFiltersInvertProcessPrivate
{
public:
    itkFiltersInvertProcessPrivate(itkFiltersInvertProcess *q) {parent = q;}
    virtual ~itkFiltersInvertProcessPrivate(void) {}

    itkFiltersInvertProcess* parent;

    template <class PixelType> void update ( void )
    {
        typedef itk::Image< PixelType, 3 > ImageType;
        typedef itk::InvertIntensityImageFilter< ImageType, ImageType >  InvertFilterType;
        typename InvertFilterType::Pointer invertFilter = InvertFilterType::New();

        invertFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( parent->input<medAbstractData*>(0)->data() ) ) );

        itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) parent );
        parent->setCallBack(callback);

        invertFilter->AddObserver ( itk::ProgressEvent(), callback );

        invertFilter->Update();
        parent->output<medAbstractData*>(0)->setData ( invertFilter->GetOutput() );

        //Set output description metadata
        QString newSeriesDescription = parent->input<medAbstractData*>(0)->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " invert filter";

        parent->output<medAbstractData*>(0)->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    }
};


itkFiltersInvertProcess::itkFiltersInvertProcess(itkFiltersInvertProcess *parent) 
    : itkFiltersProcessBase(parent), d(new itkFiltersInvertProcessPrivate(this))
{    
    this->setFilter(this);
    
    this->setDescription( tr("ITK invert intensity filter"));
}

//-------------------------------------------------------------------------------------------

itkFiltersInvertProcess::~itkFiltersInvertProcess( void )
{
}

//-------------------------------------------------------------------------------------------

bool itkFiltersInvertProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkInvertProcess", createitkFiltersInvertProcess);
}

//-------------------------------------------------------------------------------------------

int itkFiltersInvertProcess::update ( void )
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

dtkAbstractProcess * createitkFiltersInvertProcess ( void )
{
    return new itkFiltersInvertProcess;
}

