/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkFiltersBinaryCloseProcess.h"

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medMetaDataKeys.h>
#include <medAbstractImageData.h>
#include <medDataManager.h>
#include <medAbstractDataFactory.h>
#include <medAttachedData.h>

#include <itkBinaryMorphologicalClosingImageFilter.h>
#include <itkBinaryBallStructuringElement.h>

// /////////////////////////////////////////////////////////////////
// itkFiltersBinaryCloseProcessPrivate
// /////////////////////////////////////////////////////////////////

class itkFiltersBinaryCloseProcessPrivate
{
public:
    dtkSmartPointer <medAbstractData> input;
    dtkSmartPointer <medAbstractData> output;
    double radius;
};

// /////////////////////////////////////////////////////////////////
// itkFiltersBinaryCloseProcess
// /////////////////////////////////////////////////////////////////

itkFiltersBinaryCloseProcess::itkFiltersBinaryCloseProcess() : medAbstractProcess(), d(new itkFiltersBinaryCloseProcessPrivate)
{
    d->input  = NULL;
    d->output = NULL;
    d->radius = 3;
}

itkFiltersBinaryCloseProcess::~itkFiltersBinaryCloseProcess()
{
    delete d;
    d = NULL;   
}

bool itkFiltersBinaryCloseProcess::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkFiltersBinaryCloseProcess", createitkFiltersBinaryCloseProcess);
}

QString itkFiltersBinaryCloseProcess::description() const
{
    return "itkFiltersBinaryCloseProcess";
}

void itkFiltersBinaryCloseProcess::setInput (medAbstractData *data, int channel)
{
    if ( !data ) return;

    QString identifier = data->identifier();
    d->output = medAbstractDataFactory::instance()->createSmartPointer ( identifier );
    d->input = data;
}    

void itkFiltersBinaryCloseProcess::setParameter ( double  data, int channel )
{
    // Here comes a switch over channel to handle parameters
    d->radius = data;
}

// Convert medAbstractData to ITK volume
int itkFiltersBinaryCloseProcess::update()
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

template <class ImageType> int itkFiltersBinaryCloseProcess::run()
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

    typedef itk::BinaryMorphologicalClosingImageFilter <ImageType, ImageType, StructuringElementType>
            BinaryMorphologicalClosingImageFilterType;
    typename BinaryMorphologicalClosingImageFilterType::Pointer closingFilter = BinaryMorphologicalClosingImageFilterType::New();
    closingFilter->SetInput(image);
    closingFilter->SetKernel(structuringElement);
    closingFilter->Update();

    d->output->setData ( closingFilter->GetOutput() );

    if (!d->output)
        return -1;

    d->output->copyMetaDataFrom(d->input);
    QString newSeriesDescription = d->input->metadata ( medMetaDataKeys::SeriesDescription.key() );
    newSeriesDescription += " AND " + d->input->metadata ( medMetaDataKeys::SeriesDescription.key() );
    d->output->setMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );

    return EXIT_SUCCESS;
}        

medAbstractData * itkFiltersBinaryCloseProcess::output()
{
    return ( d->output );
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createitkFiltersBinaryCloseProcess()
{
    return new itkFiltersBinaryCloseProcess;
}
