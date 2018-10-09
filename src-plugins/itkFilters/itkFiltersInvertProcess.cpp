/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersInvertProcess.h>

#include <dtkCore/dtkAbstractProcessFactory.h>

#include <itkImage.h>
#include <itkInvertIntensityImageFilter.h>

#include <medUtilitiesITK.h>

itkFiltersInvertProcess::itkFiltersInvertProcess(itkFiltersInvertProcess *parent)
    : itkFiltersProcessBase(parent)
{
}

//-------------------------------------------------------------------------------------------

bool itkFiltersInvertProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkInvertProcess", createitkFiltersInvertProcess);
}

//-------------------------------------------------------------------------------------------

QString itkFiltersInvertProcess::description() const
{
    return tr("ITK invert intensity filter");
}

//-------------------------------------------------------------------------------------------

int itkFiltersInvertProcess::tryUpdate()
{
    int res = DTK_FAILURE;

    if ( getInputData() )
    {
        res = DISPATCH_ON_3D_PIXEL_TYPE(&itkFiltersInvertProcess::updateProcess, this, getInputData());
    }

    return res;
}

template <class ImageType>
int itkFiltersInvertProcess::updateProcess(medAbstractData* inputData)
{
    if (!std::is_integral<typename ImageType::PixelType>::value)
    {
        qDebug() << "Error: Invert image filter does not suport floating pixel values";
        return medAbstractProcess::PIXEL_TYPE;
    }
    else
    {
        typename ImageType::Pointer inputImage = static_cast<ImageType*>(inputData->data());

        typedef itk::InvertIntensityImageFilter< ImageType, ImageType >  InvertFilterType;
        typename InvertFilterType::Pointer invertFilter = InvertFilterType::New();

        // compute maximum intensity of image
        typedef itk::MinimumMaximumImageCalculator< ImageType > MinMaxFilterType;
        typename MinMaxFilterType::Pointer maxFilter = MinMaxFilterType::New();

        maxFilter->SetImage(inputImage);
        maxFilter->Compute();
        typename ImageType::PixelType maximum = maxFilter->GetMaximum();
        typename ImageType::PixelType minimum = maxFilter->GetMinimum();

        invertFilter->SetInput(inputImage);
        invertFilter->SetMaximum(maximum + minimum);

        itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) this );
        callback->SetCallback ( itkFiltersProcessBase::eventCallback );
        invertFilter->AddObserver ( itk::ProgressEvent(), callback );

        invertFilter->Update();

        getOutputData()->setData ( invertFilter->GetOutput() );

        //Set output description metadata
        medUtilities::setDerivedMetaData(getOutputData(), inputData, "invert filter");

        return DTK_SUCCEED;
    }
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersInvertProcess ( void )
{
    return new itkFiltersInvertProcess;
}

