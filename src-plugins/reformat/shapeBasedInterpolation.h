/*=========================================================================

 Plugin medInria - shape based interpolation

 Copyright (c) IHU LIRYC 2013. All rights reserved.

 Author : Mathilde Merle -- mathilde.merle @ ihu-liryc.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/


#pragma once

#include <itkImage.h>
#include <itkImageRegionIterator.h>
#include <itkImageRegionConstIterator.h>
#include <itkConnectedComponentImageFilter.h>
#include <itkRelabelComponentImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkExtractImageFilter.h>
#include <itkApproximateSignedDistanceMapImageFilter.h>
#include <itkReinitializeLevelSetImageFilter.h>

#include <QObject>

class shapeBasedInterpolationPrivate;
using namespace std;


class shapeBasedInterpolation : public QObject
{
    Q_OBJECT

public:
    typedef itk::Image <unsigned char, 3> MaskType;

    shapeBasedInterpolation();
    virtual ~shapeBasedInterpolation();
    void run();
    void setInput(MaskType::Pointer input0, MaskType::Pointer input1, MaskType::Pointer input2, MaskType::Pointer input3);
    MaskType::Pointer output();

signals:
    void progressed(int p);

private:
    shapeBasedInterpolationPrivate *d;
};

// PRIVATE

typedef itk::Image <unsigned char, 3>              MaskType;
typedef itk::Image <float, 3>                      MaskFloatType;
typedef itk::ImageRegionIterator <MaskType>        MaskIterator;
typedef itk::ImageRegionIterator <MaskFloatType>   MaskFloatIterator;

typedef itk::Image <unsigned char, 2>              Mask2dType;
typedef itk::ImageRegionIterator<Mask2dType>       Mask2dIterator;
typedef itk::Image <float, 2>                      Mask2dFloatType;
typedef itk::ImageRegionIterator <Mask2dFloatType> Mask2dFloatIterator;

typedef itk::ConnectedComponentImageFilter           < MaskType, MaskType >          ConnectedComponentImageFilterType;
typedef itk::RelabelComponentImageFilter             < MaskType, MaskType >          RelabelComponentImageFilterType;
typedef itk::BinaryThresholdImageFilter              < MaskType, MaskType >          BinaryThresholdImageFilterType;
typedef itk::ExtractImageFilter                      < MaskType, Mask2dType >        Extract2DType;
typedef itk::ApproximateSignedDistanceMapImageFilter < Mask2dType, Mask2dFloatType > DistanceMap2DType;
typedef itk::ReinitializeLevelSetImageFilter         < Mask2dType >                  LevelSetFilterType;


class shapeBasedInterpolationPrivate : public QObject
{
    Q_OBJECT

public:
    std::vector<MaskType::Pointer>       input;
    std::vector<MaskType::Pointer>       connectedComponent;
    std::vector<MaskType::Pointer>       interpolatedMRI;
    std::vector<MaskFloatType::Pointer>  gradientMap;
    MaskType::Pointer                    output;

    std::vector<MaskType::SpacingType>   inputSpacing;
    std::vector<MaskType::SizeType>      inputSize;
    std::vector<MaskType::PointType>     inputOrigin;
    std::vector<int>                     nbZIsotropicSlices;
    std::vector<int>                     getIntermediateSlicesNumber; // number of intermediate slices between 2 images

    /// Create volumes needed to compute final output
    void createEmptyVolumes(int inputNumber);

    /// Compute shape based interpolation volumes && interpolate MRI volumes
    void computeShapeBasedInterpolation(int inputNumber);

    /// Is there data to observe in the image ?
    unsigned short isData(Mask2dType::Pointer input);

    /// Extract a 2D slice from a 3D volume
    Mask2dType::Pointer extract2DImageSlice(MaskType::Pointer input, int plane, int slice, MaskType::SizeType size, MaskType::IndexType start);

    /// Compute a distance map from a 2D image
    Mask2dFloatType::Pointer computeDistanceMap(Mask2dType::Pointer img);

    /// Compute the interpolated slice between two distance maps
    void computeIntermediateSlice(
            Mask2dFloatType::Pointer distanceMapImg0,
            Mask2dFloatType::Pointer distanceMapImg1,
            int i,
            int j,
            int inputNumber,
            MaskIterator ito);

    /// Compute interpolated slice between two MRI slices
    void computeIntermediateMRISlice(
            Mask2dType::Pointer mri0,
            Mask2dType::Pointer mri1,
            int i,
            int j,
            int inputNumber,
            MaskIterator ito);

    /// Compute Distance Map, Gradient & Normalization on a 3D volume
    void computeGradientMap(int inputNumber);

    /// Create empty final output, and fill it
    void createFinalSeq();

    /// Compute an output pixel from computed volumes
    void computeFinalPix(MaskIterator it, int shiftingZ, int shiftingY);

signals:
    void progressed(int p);
};
