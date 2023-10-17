/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medN4BiasCorrection.h"

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>
#include <dtkCoreSupport/dtkSmartPointer.h>

#include <medAbstractDataFactory.h>
#include <medDataManager.h>
#include <medUtilities.h>
#include <medUtilitiesITK.h>

#include <itkCastImageFilter.h>
#include <itkConstantPadImageFilter.h>
#include <itkExtractImageFilter.h>
#include <itkImage.h>
#include <itkN4BiasFieldCorrectionImageFilter.h>
#include <itkOtsuThresholdImageFilter.h>
#include <itkShrinkImageFilter.h>

class medN4BiasCorrectionPrivate
{
public:
    dtkSmartPointer <medAbstractData> input;
    dtkSmartPointer <medAbstractData> mask;
    dtkSmartPointer <medAbstractData> output;

    std::vector<int> numberOfIterations, initialMeshResolution;
    double convergenceThreshold, wienerFilterNoise, saveBias;
    float bfFWHM;
    int bsplineOrder, splineDistance, shrinkFactor, nbHistogramBins;
    dtkSmartPointer<medAbstractData> biasField;

    bool cancelAsked;
};
// /////////////////////////////////////////////////////////////////
// medN4BiasCorrection
// /////////////////////////////////////////////////////////////////

medN4BiasCorrection::medN4BiasCorrection()
    : medAbstractProcessLegacy(), d(new medN4BiasCorrectionPrivate)
{
    d->output = nullptr;
    d->saveBias = false;
    d->cancelAsked = false;
}

medN4BiasCorrection::~medN4BiasCorrection()
{
    delete d;
    d = nullptr;
}

bool medN4BiasCorrection::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("medN4BiasCorrection", createMedN4BiasCorrection);
}

QString medN4BiasCorrection::description() const
{
    return "medN4BiasCorrection";
}

void medN4BiasCorrection::setInput(medAbstractData *data, int channel)
{
    if (!channel)
    {
        d->input = data;
    }
    else 
    {
        d->mask = data;
    }
}    

void medN4BiasCorrection::setParameter(double data, int channel)
{
    switch (channel)
    {
        case 0:
            d->splineDistance = data;
            break;
        case 1:
            d->bfFWHM = static_cast<float>(data);
            break;
        case 2:
            d->convergenceThreshold = data;
            break;
        case 3:
            d->bsplineOrder = data;
            break;
        case 4:
            d->shrinkFactor = data;
            break;
        case 5:
            d->nbHistogramBins = data;
            break;
        case 6:
            d->wienerFilterNoise = data;
            break;
        case 7:
            d->saveBias = data;
            break;
        default:
            if (channel >= 10)
            {
                d->numberOfIterations.push_back(data);
            }
            break;
    }
}

int medN4BiasCorrection::update()
{
    int res = medAbstractProcessLegacy::FAILURE;

    if (d->input)
    {
        res = DISPATCH_ON_3D_PIXEL_TYPE(&medN4BiasCorrection::update, this, d->input);
    }

    return res;
}        

medAbstractData * medN4BiasCorrection::output()
{
    return d->output;
}

/**
 * @brief Launch if the process is asked to be cancelled, by medRunnableProcess
 for instance. We can't quit a medRunnableProcess naturally, but some 
 checkpoints can be added in the running process to quit when reached.
 * 
 */
void medN4BiasCorrection::onCanceled()
{
    d->cancelAsked = true;
}

template <class InputImageType>
int medN4BiasCorrection::update(medAbstractData *inputData)
{
    typename InputImageType::Pointer inputImage = static_cast<InputImageType*>(inputData->data());

    typedef itk::Image<float, 3> OutputImageType;
    typedef itk::CastImageFilter<InputImageType, OutputImageType> CastType;

    typename CastType::Pointer caster = CastType::New();
    caster->SetInput(inputImage);
    typename OutputImageType::Pointer image = caster->GetOutput();

    d->output = medAbstractDataFactory::instance()->createSmartPointer("itkDataImageFloat3");
    d->biasField = medAbstractDataFactory::instance()->createSmartPointer("itkDataImageFloat3");

    typedef itk::Image<unsigned short, 3> MaskImageType;
    typename MaskImageType::Pointer maskImage;
    if(d->mask)
    {
        maskImage = dynamic_cast<MaskImageType *>((itk::Object*) (d->mask->data()));
    }
    else
    {
        maskImage = nullptr;
    }
    typedef itk::N4BiasFieldCorrectionImageFilter<OutputImageType, MaskImageType, OutputImageType> CorrecterType;

    typename CorrecterType::Pointer correcter = CorrecterType::New();
    
    if (d->cancelAsked)
    {
        return medAbstractProcessLegacy::FAILURE;
    }

    // Handle the mask image
    if( maskImage)
    {
        itk::ImageRegionConstIterator<MaskImageType> IM(
            maskImage, maskImage->GetBufferedRegion() );
        MaskImageType::PixelType maskLabel = 0;
        for( IM.GoToBegin(); !IM.IsAtEnd(); ++IM )
        {
            if (IM.Get())
            {
                maskLabel = IM.Get();
                break;
            }
        }
        if (!maskLabel)
        {
            return medAbstractProcessLegacy::FAILURE;
        }
        correcter->SetMaskLabel(maskLabel);
    }
    else
    {
        qDebug() << "N4 Bias Correction: mask not read, creating Otsu mask." << endl;
        typedef itk::OtsuThresholdImageFilter<OutputImageType, MaskImageType>
            ThresholderType;
        ThresholderType::Pointer otsu = ThresholderType::New();
        otsu->SetInput(image);
        otsu->SetNumberOfHistogramBins(200);
        otsu->SetInsideValue(0);
        otsu->SetOutsideValue(1);
        otsu->Update();
        maskImage = otsu->GetOutput();
    }

    if (d->cancelAsked)
    {
        return medAbstractProcessLegacy::FAILURE;
    }

    typename OutputImageType::Pointer weightImage = nullptr;

    // Convergence options
    d->initialMeshResolution.push_back(1); // BSpline Grid Resolution
    d->initialMeshResolution.push_back(1);
    d->initialMeshResolution.push_back(1);

    if (d->numberOfIterations.size() > 1 && d->numberOfIterations[0])
    {
        typename CorrecterType::VariableSizeArrayType
            maximumNumberOfIterations( d->numberOfIterations.size() );
        for( unsigned int i = 0; i < d->numberOfIterations.size(); i++ )
        {
            maximumNumberOfIterations[i] = d->numberOfIterations[i];
        }
        correcter->SetMaximumNumberOfIterations(maximumNumberOfIterations);

        typename CorrecterType::ArrayType numberOfFittingLevels;
        numberOfFittingLevels.Fill(d->numberOfIterations.size());
        correcter->SetNumberOfFittingLevels(numberOfFittingLevels);
    }
        
    if(d->convergenceThreshold)
    {
        correcter->SetConvergenceThreshold(d->convergenceThreshold);
    }

    typename OutputImageType::IndexType imageIndex = image->GetLargestPossibleRegion().GetIndex();
    typename OutputImageType::SizeType imageSize = image->GetLargestPossibleRegion().GetSize();
    typename OutputImageType::PointType newOrigin = image->GetOrigin();

    // B-spline options -- we place this here to take care of the case where
    // the user wants to specify things in terms of the spline distance.        
    if (d->bsplineOrder)
    {
        correcter->SetSplineOrder(d->bsplineOrder);
    }

    typename CorrecterType::ArrayType numberOfControlPoints;
    
    if (d->cancelAsked)
    {
        return medAbstractProcessLegacy::FAILURE;
    }

    if (d->splineDistance)
    {
        itk::SizeValueType lowerBound[InputImageType::ImageDimension];
        itk::SizeValueType upperBound[InputImageType::ImageDimension];

        for( unsigned int i = 0; i < 3; i++ )
        {
            float domain = static_cast<typename InputImageType::PixelType>( 
                image->GetLargestPossibleRegion().GetSize()[i] - 1 ) 
                * image->GetSpacing()[i];
            unsigned int numberOfSpans = static_cast<unsigned int>(
                std::ceil(domain / d->splineDistance));
            unsigned long extraPadding = static_cast<unsigned long>( ( numberOfSpans
                                                                       * d->splineDistance
                                                                       - domain ) / image->GetSpacing()[i] + 0.5 );
            lowerBound[i] = static_cast<unsigned long>( 0.5 * extraPadding );
            upperBound[i] = extraPadding - lowerBound[i];
            newOrigin[i] -= static_cast<typename InputImageType::PixelType>( lowerBound[i] )
                                  * image->GetSpacing()[i];
            numberOfControlPoints[i] = numberOfSpans + correcter->GetSplineOrder();
        }

        if (d->cancelAsked)
        {
            return medAbstractProcessLegacy::FAILURE;
        }

        typedef itk::ConstantPadImageFilter<OutputImageType, OutputImageType> PadderType;
        typename PadderType::Pointer padder = PadderType::New();
        padder->SetInput(image);
        padder->SetPadLowerBound(lowerBound);
        padder->SetPadUpperBound(upperBound);
        padder->SetConstant(0);
        padder->Update();
        image = padder->GetOutput();

        if (d->cancelAsked)
        {
            return medAbstractProcessLegacy::FAILURE;
        }

        typedef itk::ConstantPadImageFilter<MaskImageType, MaskImageType> MaskPadderType;
        typename MaskPadderType::Pointer maskPadder = MaskPadderType::New();
        maskPadder->SetInput(maskImage);
        maskPadder->SetPadLowerBound(lowerBound);
        maskPadder->SetPadUpperBound(upperBound);
        maskPadder->SetConstant(0);
        maskPadder->Update();
        maskImage = maskPadder->GetOutput();

        if (d->cancelAsked)
        {
            return medAbstractProcessLegacy::FAILURE;
        }

        if(weightImage)
        {
            typename PadderType::Pointer weightPadder = PadderType::New();
            weightPadder->SetInput(weightImage);
            weightPadder->SetPadLowerBound(lowerBound);
            weightPadder->SetPadUpperBound(upperBound);
            weightPadder->SetConstant(0);
            weightPadder->Update();
            weightImage = weightPadder->GetOutput();
        }
        correcter->SetNumberOfControlPoints(numberOfControlPoints);
    }
    else if( d->initialMeshResolution.size() == InputImageType::ImageDimension )
    {
        for( unsigned int i = 0; i < InputImageType::ImageDimension; i++ )
        {
            numberOfControlPoints[i] = static_cast<unsigned int>( d->initialMeshResolution[i] )
                        + correcter->GetSplineOrder();
        }
        correcter->SetNumberOfControlPoints(numberOfControlPoints);
    }

    typedef itk::ShrinkImageFilter<OutputImageType, OutputImageType> ShrinkerType;
    typename ShrinkerType::Pointer shrinker = ShrinkerType::New();
    shrinker->SetInput(image);

    typedef itk::ShrinkImageFilter<MaskImageType, MaskImageType> MaskShrinkerType;
    typename MaskShrinkerType::Pointer maskshrinker = MaskShrinkerType::New();
    maskshrinker->SetInput(maskImage);

    if (d->cancelAsked)
    {
        return medAbstractProcessLegacy::FAILURE;
    }

    shrinker->SetShrinkFactors(d->shrinkFactor);
    maskshrinker->SetShrinkFactors(d->shrinkFactor);
    shrinker->Update();
    maskshrinker->Update();

    if (d->cancelAsked)
    {
        return medAbstractProcessLegacy::FAILURE;
    }

    correcter->SetInput(shrinker->GetOutput());
    correcter->SetMaskImage(maskshrinker->GetOutput());

    // Histogram sharpening options

    if( d->bfFWHM )
    {
        correcter->SetBiasFieldFullWidthAtHalfMaximum(d->bfFWHM);
    }
        
    if( d->wienerFilterNoise )
    {
        correcter->SetWienerFilterNoise(d->wienerFilterNoise);
    }
        
    if( d->nbHistogramBins )
    {
        correcter->SetNumberOfHistogramBins(d->nbHistogramBins);
    }

    try
    {
        correcter->Update();
    }
    catch( itk::ExceptionObject & err )
    {
        std::cout << "Exception caught: " << err << std::endl;
        return medAbstractProcessLegacy::FAILURE;
    }
    catch( ... )
    {
        qDebug() << "Unknown exception caught." << endl;
        return medAbstractProcessLegacy::FAILURE;
    }
    
    if (d->cancelAsked)
    {
        return medAbstractProcessLegacy::FAILURE;
    }
    // Output
    // Reconstruct the bias field at full image resolution.  Divide
    // the original input image by the bias field to get the final
    // corrected image.

    typedef itk::BSplineControlPointImageFilter<
        CorrecterType::BiasFieldControlPointLatticeType,
        CorrecterType::ScalarImageType> BSplinerType;
    typename BSplinerType::Pointer bspliner = BSplinerType::New();
    bspliner->SetInput(correcter->GetLogBiasFieldControlPointLattice());
    bspliner->SetSplineOrder(correcter->GetSplineOrder());
    bspliner->SetSize(image->GetLargestPossibleRegion().GetSize());
    bspliner->SetOrigin(newOrigin);
    bspliner->SetDirection(image->GetDirection());
    bspliner->SetSpacing(image->GetSpacing());
    bspliner->Update();

    if (d->cancelAsked)
    {
        return medAbstractProcessLegacy::FAILURE;
    }

    typename OutputImageType::Pointer logField = OutputImageType::New();
    logField->SetOrigin(image->GetOrigin());
    logField->SetSpacing(image->GetSpacing());
    logField->SetRegions(image->GetLargestPossibleRegion());
    logField->SetDirection(image->GetDirection());
    logField->Allocate();

    itk::ImageRegionIterator<CorrecterType::ScalarImageType> IB(
        bspliner->GetOutput(),
        bspliner->GetOutput()->GetLargestPossibleRegion() );
    itk::ImageRegionIterator<OutputImageType> IF( logField,
        logField->GetLargestPossibleRegion() );
    for( IB.GoToBegin(), IF.GoToBegin(); !IB.IsAtEnd(); ++IB, ++IF )
    {
        IF.Set( IB.Get()[0] );
    }
    
    if (d->cancelAsked)
    {
        return medAbstractProcessLegacy::FAILURE;
    }

    typedef itk::ExpImageFilter<OutputImageType, OutputImageType> ExpFilterType;
    typename ExpFilterType::Pointer expFilter = ExpFilterType::New();
    expFilter->SetInput(logField);
    expFilter->Update();

    if (d->cancelAsked)
    {
        return medAbstractProcessLegacy::FAILURE;
    }

    typedef itk::DivideImageFilter<OutputImageType, OutputImageType, OutputImageType> DividerType;
    typename DividerType::Pointer divider = DividerType::New();
    divider->SetInput1(image);
    divider->SetInput2(expFilter->GetOutput());
    divider->Update();

    if (d->cancelAsked)
    {
        return medAbstractProcessLegacy::FAILURE;
    }

    typename OutputImageType::RegionType inputRegion;
    inputRegion.SetIndex(imageIndex);
    inputRegion.SetSize(imageSize);

    typedef itk::ExtractImageFilter<OutputImageType, OutputImageType> CropperType;
    typename CropperType::Pointer cropper = CropperType::New();
    cropper->SetInput(divider->GetOutput());
    cropper->SetExtractionRegion(inputRegion);
    cropper->SetDirectionCollapseToSubmatrix();
    cropper->Update();

    if (d->cancelAsked)
    {
        return medAbstractProcessLegacy::FAILURE;
    }

    typename CropperType::Pointer biasFieldCropper = CropperType::New();
    biasFieldCropper->SetInput(expFilter->GetOutput());
    biasFieldCropper->SetExtractionRegion(inputRegion);
    biasFieldCropper->SetDirectionCollapseToSubmatrix();
    biasFieldCropper->Update();

    if (d->cancelAsked)
    {
        return medAbstractProcessLegacy::FAILURE;
    }

    d->output->setData(cropper->GetOutput());
    medUtilities::setDerivedMetaData(d->output, d->input, "N4-corrected");

    if(d->saveBias)
    {
        d->biasField->setData(biasFieldCropper->GetOutput());
        medUtilities::setDerivedMetaData(d->biasField, d->input, "bias");
        medDataManager::instance().importData(d->biasField, false);
    }

    return medAbstractProcessLegacy::SUCCESS;
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createMedN4BiasCorrection()
{
    return new medN4BiasCorrection;
}
