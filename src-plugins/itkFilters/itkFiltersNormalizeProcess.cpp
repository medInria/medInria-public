/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersNormalizeProcess.h>

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <medAbstractDataFactory.h>

#include <medMetaDataKeys.h>

#include <itkNormalizeImageFilter.h>


class itkFiltersNormalizeProcessPrivate
{
public:
    itkFiltersNormalizeProcessPrivate(itkFiltersNormalizeProcess *q = 0) {parent = q;}
    virtual ~itkFiltersNormalizeProcessPrivate(void) {}

    itkFiltersNormalizeProcess *parent;

    template <class PixelType> void update ( void )
    {
        typedef itk::Image< PixelType, 3 > ImageType;
        typedef itk::NormalizeImageFilter< ImageType, ImageType >  NormalizeFilterType;
        typename NormalizeFilterType::Pointer normalizeFilter = NormalizeFilterType::New();

        normalizeFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( parent->inputImage()->data() ) ) );

        itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) parent );
        parent->setCallBack(callback);

        normalizeFilter->AddObserver ( itk::ProgressEvent(), callback );

        normalizeFilter->Update();
        parent->output()->setData ( normalizeFilter->GetOutput() );

        //Set output description metadata
        QString newSeriesDescription = parent->inputImage()->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " normalize filter";

        parent->output()->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    }

};

//-------------------------------------------------------------------------------------------

itkFiltersNormalizeProcess::itkFiltersNormalizeProcess(itkFiltersNormalizeProcess *parent) 
    : itkFiltersProcessBase(parent), d(new itkFiltersNormalizeProcessPrivate(this))
{    
    this->setFilter(this);
    
    this->setDescription(tr("ITK normalize filter"));
}

//-------------------------------------------------------------------------------------------

itkFiltersNormalizeProcess::~itkFiltersNormalizeProcess( void )
{
}

//-------------------------------------------------------------------------------------------

bool itkFiltersNormalizeProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkNormalizeProcess", createitkFiltersNormalizeProcess);
}

//-------------------------------------------------------------------------------------------

int itkFiltersNormalizeProcess::update ( void )
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

dtkAbstractProcess * createitkFiltersNormalizeProcess ( void )
{
    return new itkFiltersNormalizeProcess;
}
