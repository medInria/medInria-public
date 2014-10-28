/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersErodeProcess.h>

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <medAbstractDataFactory.h>

#include <medMetaDataKeys.h>

#include <itkGrayscaleErodeImageFilter.h>
#include <itkBinaryBallStructuringElement.h>

#include <medDoubleParameter.h>

class itkFiltersErodeProcessPrivate
{
public:
    itkFiltersErodeProcessPrivate(itkFiltersErodeProcess *q){parent = q;}

    virtual ~itkFiltersErodeProcessPrivate(void) {}

    itkFiltersErodeProcess* parent;
    medDoubleParameter *kernelSizeParam;
    QList<medAbstractParameter*> parameters;

    template <class PixelType> void update ( void )
    {
        typedef itk::Image< PixelType, 3 > ImageType;
        typedef itk::BinaryBallStructuringElement < PixelType, 3> StructuringElementType;
        typedef itk::GrayscaleErodeImageFilter< ImageType, ImageType,StructuringElementType >  ErodeType;
        typename ErodeType::Pointer erodeFilter = ErodeType::New();

        StructuringElementType ball;
        ball.SetRadius(kernelSizeParam->value());
        ball.CreateStructuringElement();

        erodeFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( parent->inputImage()->data() ) ) );
        erodeFilter->SetKernel ( ball );

        itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) parent );
        parent->setCallBack(callback);

        erodeFilter->AddObserver ( itk::ProgressEvent(), callback );
        erodeFilter->Update();
        parent->output()->setData ( erodeFilter->GetOutput() );

        QString newSeriesDescription = parent->inputImage()->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " Erode filter (" + QString::number(kernelSizeParam->value()) + ")";

        parent->output()->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    }
};

//-------------------------------------------------------------------------------------------

itkFiltersErodeProcess::itkFiltersErodeProcess(itkFiltersErodeProcess *parent) 
    : itkFiltersProcessBase(parent), d(new itkFiltersErodeProcessPrivate(this))
{    
    this->setFilter(this);

    d->kernelSizeParam = new medDoubleParameter("Kernel radius", this);
    d->kernelSizeParam->setRange(0,10);
    d->kernelSizeParam->setValue(1);
    d->kernelSizeParam->setToolTip(tr("Kernel Radius in pixels"));

    d->parameters << d->kernelSizeParam;

    this->setDescription(tr("ITK Erode filter"));
}

//-------------------------------------------------------------------------------------------

itkFiltersErodeProcess::~itkFiltersErodeProcess( void )
{
}

//-------------------------------------------------------------------------------------------

bool itkFiltersErodeProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkErodeProcess", createitkFiltersErodeProcess);
}

//-------------------------------------------------------------------------------------------

QList<medAbstractParameter*> itkFiltersErodeProcess::parameters()
{
    return d->parameters;
}

//-------------------------------------------------------------------------------------------

int itkFiltersErodeProcess::update ( void )
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

dtkAbstractProcess * createitkFiltersErodeProcess ( void )
{
    return new itkFiltersErodeProcess;
}
