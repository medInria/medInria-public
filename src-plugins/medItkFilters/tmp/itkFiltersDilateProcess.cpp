/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersDilateProcess.h>

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <medAbstractDataFactory.h>

#include <medMetaDataKeys.h>

#include <itkGrayscaleDilateImageFilter.h>
#include <itkBinaryBallStructuringElement.h>

#include <medDoubleParameter.h>


class itkFiltersDilateProcessPrivate
{
public:
    itkFiltersDilateProcessPrivate(itkFiltersDilateProcess *q){ parent = q;}

    virtual ~itkFiltersDilateProcessPrivate(void) {}

    itkFiltersDilateProcess* parent;
    medDoubleParameter *kernelSizeParam;
    QList<medAbstractParameter*> parameters;

    template <class PixelType> void update ( void )
    {
        typedef itk::Image< PixelType, 3 > ImageType;
        typedef itk::BinaryBallStructuringElement < PixelType, 3> StructuringElementType;
        typedef itk::GrayscaleDilateImageFilter< ImageType, ImageType,StructuringElementType >  DilateType;
        typename DilateType::Pointer dilateFilter = DilateType::New();

        StructuringElementType ball;
        ball.SetRadius(kernelSizeParam->value());
        ball.CreateStructuringElement();

        dilateFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( parent->input<medAbstractData*>(0)->data() ) ) );
        dilateFilter->SetKernel ( ball );

        itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) parent );
        parent->setCallBack(callback);

        dilateFilter->AddObserver ( itk::ProgressEvent(), callback );

        dilateFilter->Update();
        parent->output<medAbstractData*>(0)->setData ( dilateFilter->GetOutput() );

        QString newSeriesDescription = parent->input<medAbstractData*>(0)->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " Dilate filter (" + QString::number(kernelSizeParam->value()) + ")";

        parent->output<medAbstractData*>(0)->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    }
};

//-------------------------------------------------------------------------------------------

itkFiltersDilateProcess::itkFiltersDilateProcess(itkFiltersDilateProcess *parent) 
    : itkFiltersProcessBase(parent), d(new itkFiltersDilateProcessPrivate(this))
{    
    this->setFilter(this);

    d->kernelSizeParam = new medDoubleParameter("Kernel radius", this);
    d->kernelSizeParam->setRange(0,10);
    d->kernelSizeParam->setValue(1);
    d->kernelSizeParam->setToolTip(tr("Kernel Radius in pixels"));

    d->parameters << d->kernelSizeParam;

    this->setDescription(tr("ITK Dilate filter"));
}

//-------------------------------------------------------------------------------------------

itkFiltersDilateProcess::~itkFiltersDilateProcess( void )
{
}

//-------------------------------------------------------------------------------------------

bool itkFiltersDilateProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkDilateProcess", createitkFiltersDilateProcess);
}

//-------------------------------------------------------------------------------------------

QList<medAbstractParameter*> itkFiltersDilateProcess::parameters()
{
    return d->parameters;
}

//-------------------------------------------------------------------------------------------

int itkFiltersDilateProcess::update ( void )
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

dtkAbstractProcess * createitkFiltersDilateProcess ( void )
{
    return new itkFiltersDilateProcess;
}
