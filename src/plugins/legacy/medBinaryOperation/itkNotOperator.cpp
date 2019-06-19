/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkNotOperator.h"

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>

#include <itkBinaryNotImageFilter.h>
#include <itkMinimumMaximumImageFilter.h>

#include <medAbstractDataFactory.h>
#include <medUtilities.h>

bool itkNotOperator::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkNotOperator", createitkNotOperator);
}

QString itkNotOperator::description() const
{
    return "NOT";
}

void itkNotOperator::setInput (medAbstractData *data, int channel)
{
    if (channel == 0)
    {
        m_inputA = data;
    }
}

int itkNotOperator::update()
{
    int res = medAbstractProcessLegacy::FAILURE;

    if (m_inputA)
    {
        QString id = m_inputA->identifier();

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
            res = medAbstractProcessLegacy::PIXEL_TYPE;
        }
    }
    return res;
}


template <class ImageType>
int itkNotOperator::run()
{
    typename ImageType::Pointer inputImage = static_cast<ImageType*>(m_inputA->data());

    typedef itk::MinimumMaximumImageFilter <ImageType> ImageCalculatorFilterType;
    typename ImageCalculatorFilterType::Pointer imageCalculatorFilter = ImageCalculatorFilterType::New();
    imageCalculatorFilter->SetInput(inputImage);
    imageCalculatorFilter->Update();

    typedef itk::BinaryNotImageFilter <ImageType> NotImageFilterType;
    typename NotImageFilterType::Pointer notFilter = NotImageFilterType::New();
    notFilter->SetInput(inputImage);
    notFilter->SetBackgroundValue(imageCalculatorFilter->GetMinimum());
    notFilter->SetForegroundValue(imageCalculatorFilter->GetMaximum());
    notFilter->Update();

    QString identifier = m_inputA->identifier();
    m_output = medAbstractDataFactory::instance()->createSmartPointer(identifier);

    m_output->setData(notFilter->GetOutput());

    if (!m_output)
    {
        return medAbstractProcessLegacy::FAILURE;
    }

    medUtilities::setDerivedMetaData(m_output, m_inputA, "NOT");

    return medAbstractProcessLegacy::SUCCESS;
}        

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createitkNotOperator()
{
    return new itkNotOperator;
}
