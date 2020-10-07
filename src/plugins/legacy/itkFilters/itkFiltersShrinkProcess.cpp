/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>

#include <itkFiltersShrinkProcess.h>
#include <itkImage.h>
#include <itkShrinkImageFilter.h>

#include <medUtilities.h>
#include <medUtilitiesITK.h>

class itkFiltersShrinkProcessPrivate
{
public:
    unsigned int shrinkFactors[3];
};

const unsigned int itkFiltersShrinkProcess::defaultShrinkFactors[] = {1, 1, 1};

//-------------------------------------------------------------------------------------------

itkFiltersShrinkProcess::itkFiltersShrinkProcess(itkFiltersShrinkProcess *parent)
    : itkFiltersProcessBase(parent), d(new itkFiltersShrinkProcessPrivate)
{   
    d->shrinkFactors[0] = defaultShrinkFactors[0];
    d->shrinkFactors[1] = defaultShrinkFactors[1];
    d->shrinkFactors[2] = defaultShrinkFactors[2];
}

itkFiltersShrinkProcess::itkFiltersShrinkProcess(const itkFiltersShrinkProcess& other)
     : itkFiltersProcessBase(other), d(other.d)
{
}

//-------------------------------------------------------------------------------------------

itkFiltersShrinkProcess::~itkFiltersShrinkProcess()
{
    delete d;
}

//-------------------------------------------------------------------------------------------

bool itkFiltersShrinkProcess::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkShrinkProcess", createitkFiltersShrinkProcess);
}

//-------------------------------------------------------------------------------------------

QString itkFiltersShrinkProcess::description() const
{
    return tr("Shrink filter");
}

//-------------------------------------------------------------------------------------------

void itkFiltersShrinkProcess::setParameter(int data, int channel)
{     
    switch (channel)
    {
        case 0:
            d->shrinkFactors[0] = static_cast<unsigned int>(data);
            break;
        case 1:
            d->shrinkFactors[1] = static_cast<unsigned int>(data);
            break;
        case 2:
            d->shrinkFactors[2] = static_cast<unsigned int>(data);
            break;
        default:
            break;
    }
}

//-------------------------------------------------------------------------------------------

int itkFiltersShrinkProcess::tryUpdate()
{   
    int res = medAbstractProcessLegacy::FAILURE;

    if (getInputData())
    {
        res = DISPATCH_ON_3D_PIXEL_TYPE(&itkFiltersShrinkProcess::updateProcess, this, getInputData());
    }

    return res;
}

template <class ImageType>
int itkFiltersShrinkProcess::updateProcess(medAbstractData* inputData)
{
    typename ImageType::Pointer inputImage = static_cast<ImageType*>(inputData->data());

    typedef itk::ShrinkImageFilter< ImageType, ImageType >  ShrinkFilterType;
    typename ShrinkFilterType::Pointer shrinkFilter = ShrinkFilterType::New();

    shrinkFilter->SetInput(inputImage);
    shrinkFilter->SetShrinkFactors(d->shrinkFactors);

    itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
    callback->SetClientData(( void * ) this);
    callback->SetCallback(itkFiltersProcessBase::eventCallback);
    shrinkFilter->AddObserver(itk::ProgressEvent(), callback);

    shrinkFilter->Update();

    getOutputData()->setData(shrinkFilter->GetOutput());

    // Set output description metadata
    QString newSeriesDescription = "shrink filter " + QString::number(d->shrinkFactors[0]) + "," + QString::number(d->shrinkFactors[1]) + "," + QString::number(d->shrinkFactors[2]);
    medUtilities::setDerivedMetaData(getOutputData(), inputData, newSeriesDescription);

    return medAbstractProcessLegacy::SUCCESS;
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersShrinkProcess()
{
    return new itkFiltersShrinkProcess;
}
