/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCoreSupport/dtkSmartPointer.h>

#include <itkCastImageFilter.h>
#include <itkConstantPadImageFilter.h>
#include <itkExtractImageFilter.h>
#include <itkImage.h>
#include <itkMRIBiasFieldCorrectionFilter.h>
#include <itkN4BiasFieldCorrectionImageFilter.h>
#include <itkOtsuThresholdImageFilter.h>
#include <itkShrinkImageFilter.h>

#include <medDataManager.h>
#include <medN4BiasCorrection.h>
#include <medUtilities.h>

class medN4BiasCorrectionPrivate : public dtkAbstractProcess
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
    
    template <class InputImageType>
    int update(medAbstractData *inputData)
    {
        typename InputImageType::Pointer inputImage = static_cast<InputImageType*>(inputData->data());

        typedef itk::Image<float, 3> OutputImageType;
        typedef itk::CastImageFilter<InputImageType, OutputImageType> CastType;

        typename CastType::Pointer caster = CastType::New();
        caster->SetInput(inputImage);
        typename OutputImageType::Pointer image = caster->GetOutput();

        output = medAbstractDataFactory::instance()->createSmartPointer("itkDataImageFloat3");
        biasField = medAbstractDataFactory::instance()->createSmartPointer("itkDataImageFloat3");

        typedef itk::Image<unsigned short, 3> MaskImageType;
        typename MaskImageType::Pointer maskImage;
        if(mask)
        {
            maskImage = dynamic_cast<MaskImageType *> ( ( itk::Object* ) ( mask->data() ) );
        }
        else
        {
            maskImage = nullptr;
        }
        typedef itk::N4BiasFieldCorrectionImageFilter<OutputImageType, MaskImageType, OutputImageType> CorrecterType;

        typename CorrecterType::Pointer correcter = CorrecterType::New();

        // Handle the mask image

        if( maskImage)
        {
            itk::ImageRegionConstIterator<MaskImageType> IM(
                        maskImage, maskImage->GetBufferedRegion() );
            MaskImageType::PixelType maskLabel = 0;
            for( IM.GoToBegin(); !IM.IsAtEnd(); ++IM )
            {
                if( IM.Get() )
                {
                    maskLabel = IM.Get();
                    break;
                }
            }
            if( !maskLabel )
            {
                return medAbstractProcessLegacy::FAILURE;
            }
            correcter->SetMaskLabel(maskLabel);
        }
        else
        {
            qDebug() << "Mask not read. Creating Otsu mask." << endl;
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

        typename OutputImageType::Pointer weightImage = nullptr;

        // Convergence options

        initialMeshResolution.push_back(1); // BSpline Grid Resolution
        initialMeshResolution.push_back(1);
        initialMeshResolution.push_back(1);

        if( numberOfIterations.size() > 1 && numberOfIterations[0] )
        {
            typename CorrecterType::VariableSizeArrayType
                    maximumNumberOfIterations( numberOfIterations.size() );
            for( unsigned int i = 0; i < numberOfIterations.size(); i++ )
            {
                maximumNumberOfIterations[i] = numberOfIterations[i];
            }
            correcter->SetMaximumNumberOfIterations( maximumNumberOfIterations );

            typename CorrecterType::ArrayType numberOfFittingLevels;
            numberOfFittingLevels.Fill( numberOfIterations.size() );
            correcter->SetNumberOfFittingLevels( numberOfFittingLevels );
        }
        
        if(convergenceThreshold)
        {
            correcter->SetConvergenceThreshold(convergenceThreshold);
        }

        // B-spline options -- we place this here to take care of the case where
        // the user wants to specify things in terms of the spline distance.

        typename OutputImageType::IndexType imageIndex = image->GetLargestPossibleRegion().GetIndex();
        typename OutputImageType::SizeType imageSize = image->GetLargestPossibleRegion().GetSize();
        typename OutputImageType::PointType newOrigin = image->GetOrigin();
        
        if( bsplineOrder )
        {
            correcter->SetSplineOrder(bsplineOrder);
        }

        typename CorrecterType::ArrayType numberOfControlPoints;
        
        if( splineDistance )
        {
            itk::SizeValueType lowerBound[InputImageType::ImageDimension];
            itk::SizeValueType upperBound[InputImageType::ImageDimension];

            for( unsigned int i = 0; i < 3; i++ )
            {
                float domain = static_cast<typename InputImageType::PixelType>( image->
                                                                                GetLargestPossibleRegion().GetSize()[i] - 1 ) * image->GetSpacing()[i];
                unsigned int numberOfSpans = static_cast<unsigned int>( std::ceil( domain / splineDistance ) );
                unsigned long extraPadding = static_cast<unsigned long>( ( numberOfSpans
                                                                           * splineDistance
                                                                           - domain ) / image->GetSpacing()[i] + 0.5 );
                lowerBound[i] = static_cast<unsigned long>( 0.5 * extraPadding );
                upperBound[i] = extraPadding - lowerBound[i];
                newOrigin[i] -= ( static_cast<typename InputImageType::PixelType>( lowerBound[i] )
                                  * image->GetSpacing()[i] );
                numberOfControlPoints[i] = numberOfSpans + correcter->GetSplineOrder();
            }

            typedef itk::ConstantPadImageFilter<OutputImageType, OutputImageType> PadderType;
            typename PadderType::Pointer padder = PadderType::New();
            padder->SetInput(image);
            padder->SetPadLowerBound(lowerBound);
            padder->SetPadUpperBound(upperBound);
            padder->SetConstant(0);
            padder->Update();
            image = padder->GetOutput();

            typedef itk::ConstantPadImageFilter<MaskImageType, MaskImageType> MaskPadderType;
            typename MaskPadderType::Pointer maskPadder = MaskPadderType::New();
            maskPadder->SetInput(maskImage);
            maskPadder->SetPadLowerBound(lowerBound);
            maskPadder->SetPadUpperBound(upperBound);
            maskPadder->SetConstant(0);
            maskPadder->Update();
            maskImage = maskPadder->GetOutput();

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
        else if( initialMeshResolution.size() == InputImageType::ImageDimension )
        {
            for( unsigned int d = 0; d < InputImageType::ImageDimension; d++ )
            {
                numberOfControlPoints[d] = static_cast<unsigned int>( initialMeshResolution[d] )
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

        shrinker->SetShrinkFactors(shrinkFactor);
        maskshrinker->SetShrinkFactors(shrinkFactor);
        shrinker->Update();
        maskshrinker->Update();

        correcter->SetInput(shrinker->GetOutput());
        correcter->SetMaskImage(maskshrinker->GetOutput());

        // Histogram sharpening options

        if( bfFWHM )
        {
            correcter->SetBiasFieldFullWidthAtHalfMaximum(bfFWHM);
        }
        
        if( wienerFilterNoise )
        {
            correcter->SetWienerFilterNoise(wienerFilterNoise);
        }
        
        if( nbHistogramBins )
        {
            correcter->SetNumberOfHistogramBins(nbHistogramBins);
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

        typedef itk::ExpImageFilter<OutputImageType, OutputImageType> ExpFilterType;
        typename ExpFilterType::Pointer expFilter = ExpFilterType::New();
        expFilter->SetInput(logField);
        expFilter->Update();

        typedef itk::DivideImageFilter<OutputImageType, OutputImageType, OutputImageType> DividerType;
        typename DividerType::Pointer divider = DividerType::New();
        divider->SetInput1(image);
        divider->SetInput2(expFilter->GetOutput());
        divider->Update();

        typename OutputImageType::RegionType inputRegion;
        inputRegion.SetIndex(imageIndex);
        inputRegion.SetSize(imageSize);

        typedef itk::ExtractImageFilter<OutputImageType, OutputImageType> CropperType;
        typename CropperType::Pointer cropper = CropperType::New();
        cropper->SetInput(divider->GetOutput());
        cropper->SetExtractionRegion(inputRegion);
        cropper->SetDirectionCollapseToSubmatrix();
        cropper->Update();

        typename CropperType::Pointer biasFieldCropper = CropperType::New();
        biasFieldCropper->SetInput(expFilter->GetOutput());
        biasFieldCropper->SetExtractionRegion(inputRegion);
        biasFieldCropper->SetDirectionCollapseToSubmatrix();

        biasFieldCropper->Update();

        output->setData(cropper->GetOutput());
        medUtilities::setDerivedMetaData(output, input, "N4-corrected");

        if(saveBias)
        {
            biasField->setData(biasFieldCropper->GetOutput());
            medUtilities::setDerivedMetaData(biasField, input, "bias");
            medDataManager::instance()->importData(biasField, false);
        }

        return medAbstractProcessLegacy::SUCCESS;
    }
};
