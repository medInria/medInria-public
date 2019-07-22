/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
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
#include <medUtilitiesITK.h>

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
        res = DISPATCH_ON_3D_PIXEL_TYPE(&itkNotOperator::run, this, m_inputA);
    }
    return res;
}


template <class ImageType>
int itkNotOperator::run(medAbstractData* inputData)
{
    typename ImageType::Pointer inputImage = static_cast<ImageType*>(inputData->data());

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

    QString identifier = inputData->identifier();
    m_output = medAbstractDataFactory::instance()->createSmartPointer(identifier);

    m_output->setData(notFilter->GetOutput());

    if (!m_output)
    {
        return medAbstractProcessLegacy::FAILURE;
    }

    medUtilities::setDerivedMetaData(m_output, inputData, "NOT");

    return medAbstractProcessLegacy::SUCCESS;
}        

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createitkNotOperator()
{
    return new itkNotOperator;
}
