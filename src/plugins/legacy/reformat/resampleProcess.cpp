/*=========================================================================

 medInria

 Copyright (c) INRIA 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>

#include <itkResampleImageFilter.h>
#include <itkBSplineInterpolateImageFunction.h>

#include <medAbstractDataFactory.h>
#include <medAbstractProcess.h>
#include <medMetaDataKeys.h>
#include <medUtilities.h>
#include <medUtilitiesITK.h>

#include <resampleProcess.h>

// /////////////////////////////////////////////////////////////////
// resampleProcessPrivate
// /////////////////////////////////////////////////////////////////
class resampleProcessPrivate
{
public:
    resampleProcess *parent;
    resampleProcessPrivate(resampleProcess *p)
    {
        parent = p;
    }
    medAbstractData* input;
    medAbstractData* output;
    int interpolator;
    int dimX, dimY, dimZ;
    double spacingX, spacingY, spacingZ;
};

// /////////////////////////////////////////////////////////////////
// resampleProcess
// /////////////////////////////////////////////////////////////////
resampleProcess::resampleProcess(void) : d(new resampleProcessPrivate(this))
{
    d->dimX = 0;
    d->dimY = 0;
    d->dimZ = 0;
    d->spacingX = 0.0;
    d->spacingY = 0.0;
    d->spacingZ = 0.0;
}

resampleProcess::~resampleProcess(void)
{
    delete d;
    d = nullptr;
}

bool resampleProcess::registered(void)
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("resampleProcess", createResampleProcess);
}

QString resampleProcess::description(void) const
{
    return "resampleProcess";
}

void resampleProcess::setInput ( medAbstractData *data)
{
    if ( data )
    {
        d->input = data;
    }
}

void resampleProcess::setInput ( medAbstractData *data , int channel)
{
    setInput(data);
}

void resampleProcess::setParameter ( double data, int channel)
{
    switch (channel)
    {
    case 0:
        d->dimX = static_cast<int>(data);
        break;
    case 1:
        d->dimY = static_cast<int>(data);
        break;
    case 2:
        d->dimZ = static_cast<int>(data);
        break;
    case 3:
        d->spacingX = data;
        break;
    case 4:
        d->spacingY = data;
        break;
    case 5:
        d->spacingZ = data;
        break;
    }
}

int resampleProcess::update ( void )
{
    int result = medAbstractProcessLegacy::FAILURE;

    if (!d->input)
    {
        qDebug() << "in update method: d->input is null";
    }
    else
    {
        result = DISPATCH_ON_3D_PIXEL_TYPE(&resampleProcess::resample, this, d->input);

        if (result == medAbstractProcessLegacy::PIXEL_TYPE)
        {
            result = DISPATCH_ON_4D_PIXEL_TYPE(&resampleProcess::resample, this, d->input);
        }
    }

    return result;
}

template <class ImageType>
int resampleProcess::resample(medAbstractData* inputData)
{
    typename ImageType::Pointer inputImage = static_cast<ImageType*>(inputData->data());

    typedef typename itk::ResampleImageFilter<ImageType, ImageType,double> ResampleFilterType;

    typename ResampleFilterType::Pointer resampleFilter = ResampleFilterType::New();

    // Fetch original image size.
    const typename ImageType::RegionType& inputRegion = inputImage->GetLargestPossibleRegion();
    const typename ImageType::SizeType& vnInputSize = inputRegion.GetSize();
    unsigned int nOldX = vnInputSize[0];
    unsigned int nOldY = vnInputSize[1];
    unsigned int nOldZ = vnInputSize[2];

    // Fetch original image spacing.
    const typename ImageType::SpacingType& vfInputSpacing = inputImage->GetSpacing();
    double vfOutputSpacing[3] = {d->spacingX,d->spacingY,d->spacingZ};
    if (d->dimX || d->dimY || d->dimZ)
    {
        vfOutputSpacing[0] = vfInputSpacing[0] * (double) nOldX / (double) d->dimX;
        vfOutputSpacing[1] = vfInputSpacing[1] * (double) nOldY / (double) d->dimY;
        vfOutputSpacing[2] = vfInputSpacing[2] * (double) nOldZ / (double) d->dimZ;
    }
    else
    {
        d->dimX =  floor((vfInputSpacing[0] * (double) nOldX / (double) vfOutputSpacing[0]) +0.5);
        d->dimY =  floor((vfInputSpacing[1] * (double) nOldY / (double) vfOutputSpacing[1]) +0.5);
        d->dimZ =  floor((vfInputSpacing[2] * (double) nOldZ / (double) vfOutputSpacing[2]) +0.5);
    }

    typename ImageType::SizeType vnOutputSize;
    vnOutputSize[0] = d->dimX;
    vnOutputSize[1] = d->dimY;
    vnOutputSize[2] = d->dimZ;

    resampleFilter->SetInput(inputImage);
    resampleFilter->SetSize(vnOutputSize);
    resampleFilter->SetOutputSpacing(vfOutputSpacing);
    resampleFilter->SetOutputOrigin( inputImage->GetOrigin() );
    resampleFilter->SetOutputDirection(inputImage->GetDirection() );
    resampleFilter->UpdateLargestPossibleRegion();

    d->output = medAbstractDataFactory::instance()->create(inputData->identifier());
    d->output->setData(resampleFilter->GetOutput());

    medUtilities::setDerivedMetaData(d->output, inputData, "resampled");

    return medAbstractProcessLegacy::SUCCESS;
}

medAbstractData * resampleProcess::output ( void )
{
    return d->output;
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////
dtkAbstractProcess *createResampleProcess(void)
{
    return new resampleProcess;
}
