/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkAndOperator.h"

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medMetaDataKeys.h>
#include <medAbstractImageData.h>
#include <medDataManager.h>
#include <medAbstractDataFactory.h>
#include <medUtilities.h>

#include <itkCastImageFilter.h>
#include <itkAndImageFilter.h>

// /////////////////////////////////////////////////////////////////
// itkAndOperatorPrivate
// /////////////////////////////////////////////////////////////////

class itkAndOperatorPrivate
{
public:
    dtkSmartPointer <medAbstractData> inputA;
    dtkSmartPointer <medAbstractData> inputB;
    dtkSmartPointer <medAbstractData> output;
};

// /////////////////////////////////////////////////////////////////
// itkAndOperator
// /////////////////////////////////////////////////////////////////

itkAndOperator::itkAndOperator() : medAbstractProcess(), d(new itkAndOperatorPrivate)
{
    d->inputA = NULL;
    d->inputB = NULL;
    d->output = NULL;   
}

itkAndOperator::~itkAndOperator()
{
    delete d;
    d = NULL;
}

bool itkAndOperator::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkAndOperator", createitkAndOperator);
}

QString itkAndOperator::description() const
{
    return "itkAndOperator";
}

void itkAndOperator::setInput ( medAbstractData *data, int channel)
{
    if ( channel == 0)
    {
        d->inputA = data;
    }

    if ( channel == 1 )
    {        
        d->output = medAbstractDataFactory::instance()->createSmartPointer ( "itkDataImageUChar3" );
        d->inputB = data;
    }
}    

void itkAndOperator::setParameter ( double  data, int channel )
{
    // Here comes a switch over channel to handle parameters
}

int itkAndOperator::update()
{
    int res = EXIT_FAILURE;

    if (!d->inputA)
    {
        return res;
    }

    QString id = d->inputA->identifier();

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
    return res;
}

template <class ImageType> int itkAndOperator::run()
{
    int res = EXIT_FAILURE;

    if (!d->inputB)
    {
        return res;
    }

    QString id = d->inputB->identifier();

    if ( id == "itkDataImageChar3" )
    {
        res = runProcess< ImageType, itk::Image <char,3> >();
    }
    else if ( id == "itkDataImageUChar3" )
    {
        res = runProcess< ImageType, itk::Image <unsigned char,3> >();
    }
    else if ( id == "itkDataImageShort3" )
    {
        res = runProcess< ImageType, itk::Image <short,3> >();
    }
    else if ( id == "itkDataImageUShort3" )
    {
        res = runProcess< ImageType, itk::Image <unsigned short,3> >();
    }
    else if ( id == "itkDataImageInt3" )
    {
        res = runProcess< ImageType, itk::Image <int,3> >();
    }
    else if ( id == "itkDataImageUInt3" )
    {
        res = runProcess< ImageType, itk::Image <unsigned int,3> >();
    }
    else if ( id == "itkDataImageLong3" )
    {
        res = runProcess< ImageType, itk::Image <long,3> >();
    }
    else if ( id== "itkDataImageULong3" )
    {
        res = runProcess< ImageType, itk::Image <unsigned long,3> >();
    }
    else if ( id == "itkDataImageFloat3" )
    {
        res = runProcess< ImageType, itk::Image <float,3> >();
    }
    else if ( id == "itkDataImageDouble3" )
    {
        res = EXIT_FAILURE; //runProcess< ImageType, itk::Image <double,3> >();
    }
    else
    {
        qDebug() << description()
                 <<", Error : pixel type not yet implemented ("
                 << id
                 << ")";
    }
    return res;
}

template <class ImageType, class ImageType2> int itkAndOperator::runProcess()
{
    if ( !d->inputA->data() || !d->inputB->data())
    {
        return EXIT_FAILURE;
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    typedef itk::Image<unsigned char, 3> ImageTypeOutput;
    ImageTypeOutput::Pointer imageA;
    ImageTypeOutput::Pointer imageB;

    if (d->inputA->identifier() != "itkDataImageUChar3")
    {
        typename ImageType::Pointer imA = dynamic_cast< ImageType*>((itk::Object*)(d->inputA->data()));
        typedef itk::CastImageFilter< ImageType, ImageTypeOutput > CasterType;
        typename CasterType::Pointer caster = CasterType::New();
        caster->SetInput(imA);
        caster->Update();
        imageA = caster->GetOutput();
    } else
    {
        imageA = dynamic_cast< ImageTypeOutput*>((itk::Object*)(d->inputA->data()));
    }

    if (d->inputB->identifier() != "itkDataImageUChar3")
    {
        typename ImageType2::Pointer imB = dynamic_cast< ImageType2*>((itk::Object*)(d->inputB->data()));
        typedef itk::CastImageFilter< ImageType2, ImageTypeOutput > CasterType;
        typename CasterType::Pointer caster = CasterType::New();
        caster->SetInput(imB);
        caster->Update();
        imageB = caster->GetOutput();
    } else
    {
        imageB = dynamic_cast< ImageTypeOutput*>((itk::Object*)(d->inputB->data()));
    }

    if (!imageA || !imageB)
    {
        return EXIT_FAILURE;
    }

    try
    {
        typedef itk::AndImageFilter <ImageTypeOutput, ImageTypeOutput, ImageTypeOutput> AndImageFilterType;
        AndImageFilterType::Pointer andFilter = AndImageFilterType::New();
        andFilter->SetInput1(imageA);
        andFilter->SetInput2(imageB);
        andFilter->Update();
        d->output->setData(andFilter->GetOutput());
    }
    catch( itk::ExceptionObject & err )
    {
        std::cerr << "ExceptionObject caught in "<< metaObject()->className() << std::endl;
        std::cerr << err << std::endl;
        QApplication::restoreOverrideCursor();
        QApplication::processEvents();
        return EXIT_FAILURE;
    }

    QString derivedDescription = "AND ";
    derivedDescription += d->inputB->metadata(medMetaDataKeys::SeriesDescription.key());
    medUtilities::setDerivedMetaData(d->output, d->inputA, derivedDescription);

    QApplication::restoreOverrideCursor();
    QApplication::processEvents();

    return EXIT_SUCCESS;
}        

medAbstractData * itkAndOperator::output()
{
    return ( d->output );
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createitkAndOperator()
{
    return new itkAndOperator;
}
