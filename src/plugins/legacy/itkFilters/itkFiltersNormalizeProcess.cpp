/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>

#include <itkFiltersNormalizeProcess.h>
#include <itkImage.h>
#include <itkNormalizeImageFilter.h>

#include <medUtilities.h>
#include <medUtilitiesITK.h>

//-------------------------------------------------------------------------------------------

itkFiltersNormalizeProcess::itkFiltersNormalizeProcess(itkFiltersNormalizeProcess *parent) 
    : itkFiltersProcessBase(parent)
{
}

//-------------------------------------------------------------------------------------------

bool itkFiltersNormalizeProcess::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkNormalizeProcess", createitkFiltersNormalizeProcess);
}

//-------------------------------------------------------------------------------------------

QString itkFiltersNormalizeProcess::description() const
{
    return tr("Normalize filter");
}

//-------------------------------------------------------------------------------------------

int itkFiltersNormalizeProcess::tryUpdate()
{   
    int res = medAbstractProcessLegacy::FAILURE;

    if (getInputData())
    {
        res = DISPATCH_ON_3D_PIXEL_TYPE(&itkFiltersNormalizeProcess::updateProcess, this, getInputData());
    }

    return res;
}

template <class ImageType>
int itkFiltersNormalizeProcess::updateProcess(medAbstractData* inputData)
{
    typename ImageType::Pointer inputImage = static_cast<ImageType*>(inputData->data());

    typedef itk::NormalizeImageFilter< ImageType, ImageType >  NormalizeFilterType;
    typename NormalizeFilterType::Pointer normalizeFilter = NormalizeFilterType::New();

    normalizeFilter->SetInput(inputImage);

    itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
    callback->SetClientData(( void * ) this);
    callback->SetCallback(itkFiltersProcessBase::eventCallback);
    normalizeFilter->AddObserver(itk::ProgressEvent(), callback);

    normalizeFilter->Update();

    getOutputData()->setData(normalizeFilter->GetOutput());

    // Set output description metadata
    medUtilities::setDerivedMetaData(getOutputData(), inputData, "normalize filter");

    return medAbstractProcessLegacy::SUCCESS;
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersNormalizeProcess()
{
    return new itkFiltersNormalizeProcess;
}
