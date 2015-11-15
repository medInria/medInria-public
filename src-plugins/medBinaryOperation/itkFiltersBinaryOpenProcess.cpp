/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkFiltersBinaryOpenProcess.h"

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medMetaDataKeys.h>
#include <medAbstractImageData.h>
#include <medDataManager.h>
#include <medAbstractDataFactory.h>
#include <medAttachedData.h>

#include <itkBinaryMorphologicalOpeningImageFilter.h>
#include <itkBinaryBallStructuringElement.h>

// /////////////////////////////////////////////////////////////////
// itkFiltersBinaryOpenProcessPrivate
// /////////////////////////////////////////////////////////////////

class itkFiltersBinaryOpenProcessPrivate
{
public:
    dtkSmartPointer <medAbstractData> input;
    dtkSmartPointer <medAbstractData> output;
    double radius;
};

// /////////////////////////////////////////////////////////////////
// itkFiltersBinaryOpenProcess
// /////////////////////////////////////////////////////////////////

itkFiltersBinaryOpenProcess::itkFiltersBinaryOpenProcess() : medAbstractProcess(), d(new itkFiltersBinaryOpenProcessPrivate)
{
    d->input  = NULL;
    d->output = NULL;
    d->radius = 3;
}

itkFiltersBinaryOpenProcess::~itkFiltersBinaryOpenProcess()
{
    delete d;
    d = NULL;
}

bool itkFiltersBinaryOpenProcess::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkFiltersBinaryOpenProcess", createitkFiltersBinaryOpenProcess);
}

QString itkFiltersBinaryOpenProcess::description() const
{
    return "itkFiltersBinaryOpenProcess";
}

void itkFiltersBinaryOpenProcess::setInput (medAbstractData *data, int channel)
{
    if ( !data ) return;

    QString identifier = data->identifier();
    d->output = medAbstractDataFactory::instance()->createSmartPointer ( identifier );
    d->input = data;
}    

void itkFiltersBinaryOpenProcess::setParameter ( double  data, int channel )
{
    // Here comes a switch over channel to handle parameters
    d->radius = data;
}

// Convert medAbstractData to ITK volume
int itkFiltersBinaryOpenProcess::update()
{
    int res = 0;

    QString id = d->input->identifier();

    if ( id == "itkDataImageChar3" )
    {
        res = run< itk::Image <char,3> >();
    }
    else if ( id == "itkDataImageUChar3" )
    {
        res = run< itk::Image <unsigned char,3> >();
    }
    else if ( id == "itkDataImageShort3" )
    {
        res = run< itk::Image <short,3> >();
    }
    else if ( id == "itkDataImageUShort3" )
    {
        res = run< itk::Image <unsigned short,3> >();
    }
    else if ( id == "itkDataImageInt3" )
    {
        res = run< itk::Image <int,3> >();
    }
    else if ( id == "itkDataImageUInt3" )
    {
        res = run< itk::Image <unsigned int,3> >();
    }
    else if ( id == "itkDataImageLong3" )
    {
        res = run< itk::Image <long,3> >();
    }
    else if ( id== "itkDataImageULong3" )
    {
        res = run< itk::Image <unsigned long,3> >();
    }
    else if ( id == "itkDataImageFloat3" )
    {
        res = run< itk::Image <float,3> >();
    }
    else if ( id == "itkDataImageDouble3" )
    {
        res = run< itk::Image <double,3> >();
    }
    else
    {
        qDebug() << "Error : pixel type not yet implemented ("
        << id
        << ")";
    }
    return res;
}

template <class ImageType> int itkFiltersBinaryOpenProcess::run()
{
    if ( !d->input ||!d->input->data())
        return -1;

    typename ImageType::Pointer image = dynamic_cast<ImageType  *> ( ( itk::Object* ) ( d->input->data() )) ;

    if (!image)
        return -1;

    typedef itk::BinaryBallStructuringElement<typename ImageType::PixelType, 3> StructuringElementType;
    StructuringElementType structuringElement;
    structuringElement.SetRadius((unsigned int)d->radius);
    structuringElement.CreateStructuringElement();

    typedef itk::BinaryMorphologicalOpeningImageFilter <ImageType, ImageType, StructuringElementType>
            BinaryMorphologicalOpeningImageFilterType;
    typename BinaryMorphologicalOpeningImageFilterType::Pointer openingFilter = BinaryMorphologicalOpeningImageFilterType::New();
    openingFilter->SetInput(image);
    openingFilter->SetKernel(structuringElement);
    openingFilter->Update();

    d->output->setData ( openingFilter->GetOutput() );

    if (!d->output)
        return -1;

    d->output->copyMetaDataFrom(d->input);
    QString newSeriesDescription = d->input->metadata ( medMetaDataKeys::SeriesDescription.key() );
    newSeriesDescription += " AND " + d->input->metadata ( medMetaDataKeys::SeriesDescription.key() );
    d->output->setMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );

    return EXIT_SUCCESS;
}        

medAbstractData * itkFiltersBinaryOpenProcess::output()
{
    return ( d->output );
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createitkFiltersBinaryOpenProcess()
{
    return new itkFiltersBinaryOpenProcess;
}
