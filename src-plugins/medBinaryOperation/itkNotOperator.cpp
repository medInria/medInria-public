/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkNotOperator.h"

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medAbstractImageData.h>
#include <medDataManager.h>
#include <medAbstractDataFactory.h>
#include <medAttachedData.h>
#include <medUtilities.h>

#include <itkBinaryNotImageFilter.h>
#include <itkMinimumMaximumImageFilter.h>

// /////////////////////////////////////////////////////////////////
// itkNotOperatorPrivate
// /////////////////////////////////////////////////////////////////

class itkNotOperatorPrivate
{
public:
    dtkSmartPointer <medAbstractData> input;
    dtkSmartPointer <medAbstractData> output;
};

// /////////////////////////////////////////////////////////////////
// itkNotOperator
// /////////////////////////////////////////////////////////////////

itkNotOperator::itkNotOperator() : medAbstractProcess(), d(new itkNotOperatorPrivate)
{
    d->input  = NULL;
    d->output = NULL;
}

itkNotOperator::~itkNotOperator()
{
    delete d;
    d = NULL;
}

bool itkNotOperator::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkNotOperator", createitkNotOperator);
}

QString itkNotOperator::description() const
{
    return "itkNotOperator";
}

void itkNotOperator::setInput (medAbstractData *data, int channel)
{
    if (channel == 0)
    {
        d->input = data;
        if ( data )
        {
            QString identifier = data->identifier();
            d->output = medAbstractDataFactory::instance()->createSmartPointer ( identifier );
        }
        else
        {
            d->output = NULL;
        }
    }
}    

void itkNotOperator::setParameter ( double  data, int channel )
{

}

// Convert medAbstractData to ITK volume
int itkNotOperator::update()
{
    int res = DTK_FAILURE;

    if (d->input)
    {
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
            qDebug() << description()
                     <<", Error : pixel type not yet implemented ("
                     << id
                     << ")";
        }
    }
    return res;
}

template <class ImageType> int itkNotOperator::run()
{
    if ( !d->input->data() )
    {
        return DTK_FAILURE;
    }
    typename ImageType::Pointer image = dynamic_cast<ImageType  *> ( ( itk::Object* ) ( d->input->data() )) ;

    if (!image)
    {
        return DTK_FAILURE;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

#if ITK_VERSION_MAJOR >= 4

    typedef itk::MinimumMaximumImageFilter <ImageType> ImageCalculatorFilterType;
    typename ImageCalculatorFilterType::Pointer imageCalculatorFilter = ImageCalculatorFilterType::New();
    imageCalculatorFilter->SetInput(image);
    imageCalculatorFilter->Update();

    typedef itk::BinaryNotImageFilter <ImageType> NotImageFilterType;
    typename NotImageFilterType::Pointer notFilter = NotImageFilterType::New();
    notFilter->SetInput(image);
    notFilter->SetBackgroundValue(imageCalculatorFilter->GetMinimum());
    notFilter->SetForegroundValue(imageCalculatorFilter->GetMaximum());
    notFilter->Update();

    d->output->setData(notFilter->GetOutput());
#endif

    QApplication::restoreOverrideCursor();
    QApplication::processEvents();

    if (!d->output)
    {
        return DTK_FAILURE;
    }

    medUtilities::setDerivedMetaData(d->output, d->input, "NOT");

    return DTK_SUCCEED;
}        

medAbstractData * itkNotOperator::output()
{
    return ( d->output );
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createitkNotOperator()
{
    return new itkNotOperator;
}
