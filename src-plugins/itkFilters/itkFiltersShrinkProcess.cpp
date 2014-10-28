/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersShrinkProcess.h>

#include <dtkCore/dtkAbstractProcessFactory.h>

#include <medMetaDataKeys.h>
#include <medIntParameter.h>

#include <itkShrinkImageFilter.h>

class itkFiltersShrinkProcessPrivate
{
public:
    itkFiltersShrinkProcessPrivate(itkFiltersShrinkProcess *q) {parent=q;}
    virtual ~itkFiltersShrinkProcessPrivate(void) {}

    itkFiltersShrinkProcess* parent;
    medIntParameter *shrinkParam1;
    medIntParameter *shrinkParam2;
    medIntParameter *shrinkParam3;
    QList<medAbstractParameter*> parameters;
    unsigned int shrinkFactors[3];

    template <class PixelType> void update ( void )
    {
        typedef itk::Image< PixelType, 3 > ImageType;
        typedef itk::ShrinkImageFilter< ImageType, ImageType >  ShrinkFilterType;
        typename ShrinkFilterType::Pointer shrinkFilter = ShrinkFilterType::New();

        shrinkFactors[0] = shrinkParam1->value();
        shrinkFactors[1] = shrinkParam2->value();
        shrinkFactors[2] = shrinkParam3->value();

        shrinkFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( parent->inputImage()->data() ) ) );
        shrinkFilter->SetShrinkFactors(shrinkFactors);

        itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) parent );
        parent->setCallBack(callback);

        shrinkFilter->AddObserver ( itk::ProgressEvent(), callback );

        shrinkFilter->Update();
        parent->output()->setData ( shrinkFilter->GetOutput() );

        //Set output description metadata
        QString newSeriesDescription = parent->inputImage()->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " shrink filter (" + QString::number(shrinkFactors[0]) + "," + QString::number(shrinkFactors[1]) + "," + QString::number(shrinkFactors[2]) + ")";

        parent->output()->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    }

};

//-------------------------------------------------------------------------------------------

itkFiltersShrinkProcess::itkFiltersShrinkProcess(itkFiltersShrinkProcess *parent) 
    : itkFiltersProcessBase(parent), d(new itkFiltersShrinkProcessPrivate(this))
{    
    this->setFilter(this);
    
    this->setDescription(tr("ITK shrink filter"));

    d->shrinkParam1 = new medIntParameter("Shrink factor X", this);
    d->shrinkParam1->setRange(1,10);
    d->shrinkParam1->setValue(1);

    d->shrinkParam2 = new medIntParameter("Shrink factor Y", this);
    d->shrinkParam2->setRange(1,10);
    d->shrinkParam2->setValue(1);

    d->shrinkParam3 = new medIntParameter("Shrink factor Z", this);
    d->shrinkParam3->setRange(1,10);
    d->shrinkParam3->setValue(1);

    d->parameters << d->shrinkParam1 << d->shrinkParam2 << d->shrinkParam3;
}

//-------------------------------------------------------------------------------------------

itkFiltersShrinkProcess::~itkFiltersShrinkProcess( void )
{
}

//-------------------------------------------------------------------------------------------

bool itkFiltersShrinkProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkShrinkProcess", createitkFiltersShrinkProcess);
}

//-------------------------------------------------------------------------------------------

QList<medAbstractParameter*> itkFiltersShrinkProcess::parameters()
{
    return d->parameters;
}

//-------------------------------------------------------------------------------------------

int itkFiltersShrinkProcess::update ( void )
{    
    if ( !this->inputImage() )
        return -1;

    QString id = this->inputImage()->identifier();

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

dtkAbstractProcess * createitkFiltersShrinkProcess ( void )
{
    return new itkFiltersShrinkProcess;
}
