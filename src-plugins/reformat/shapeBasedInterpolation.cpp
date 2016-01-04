/*=========================================================================

 Plugin medInria - shape based interpolation

 Copyright (c) IHU LIRYC 2013. All rights reserved.

 Author : Mathilde Merle -- mathilde.merle @ ihu-liryc.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "shapeBasedInterpolation.h"

#include <algorithm>    // std::max
#include <math.h>       /* pow, sqrt */

#include <itkBinaryDilateImageFilter.h>
#include <itkBinaryErodeImageFilter.h>
#include <itkBinaryBallStructuringElement.h>
#include <itkDerivativeImageFilter.h>
#include <itkNormalizeImageFilter.h>
#include <itkSignedMaurerDistanceMapImageFilter.h>


// Create volumes needed to compute final output
void shapeBasedInterpolationPrivate::createEmptyVolumes(int inputNumber)
{
    cout<<"shapeBasedInterpolationPrivate::createEmptyVolumes"<<endl;

    // Get meta data from input.
    this->inputSpacing.push_back(this->input[inputNumber]->GetSpacing());
    this->inputSize.push_back(   this->input[inputNumber]->GetLargestPossibleRegion().GetSize());
    this->inputOrigin.push_back( this->input[inputNumber]->GetOrigin());
    this->nbZIsotropicSlices.push_back(this->inputSize[inputNumber][2] *
            (this->inputSpacing[inputNumber][2] / this->inputSpacing[inputNumber][0]));

    // Create region
    MaskType::IndexType start;
    start.Fill(0);
    MaskType::SizeType size; //create the cube
    size[0] = this->inputSize[inputNumber][0];
    size[1] = this->inputSize[inputNumber][1];
    size[2] = this->nbZIsotropicSlices[inputNumber];
    MaskType::RegionType region(start, size);

    // Initialize new spacing
    MaskType::SpacingType outputSpacing;
    outputSpacing[0] = this->inputSpacing[inputNumber][0];
    outputSpacing[1] = this->inputSpacing[inputNumber][1];
    outputSpacing[2] = this->inputSpacing[inputNumber][0];

    // Create Empty ConnectedComponent volumes
    MaskType::Pointer currentConnectedComponent = MaskType::New();
    currentConnectedComponent->SetRegions(region);
    currentConnectedComponent->Allocate();
    currentConnectedComponent->FillBuffer(0);
    currentConnectedComponent->SetSpacing(outputSpacing);
    currentConnectedComponent->SetOrigin(this->inputOrigin[inputNumber]);
    currentConnectedComponent->SetDirection(this->input[inputNumber]->GetDirection());
    this->connectedComponent.push_back(currentConnectedComponent);

    // Create Empty interpolatedMRI volumes
    MaskType::Pointer currentInterpolatedMRI = MaskType::New();
    currentInterpolatedMRI->SetRegions(region);
    currentInterpolatedMRI->Allocate();
    currentInterpolatedMRI->FillBuffer(0);
    currentInterpolatedMRI->SetSpacing(outputSpacing);
    currentInterpolatedMRI->SetOrigin(this->input[inputNumber+2]->GetOrigin());
    currentInterpolatedMRI->SetDirection(this->input[inputNumber+2]->GetDirection());
    this->interpolatedMRI.push_back(currentInterpolatedMRI);

    this->getIntermediateSlicesNumber.push_back((int)(this->inputSpacing[inputNumber][2] / this->inputSpacing[inputNumber][0] + 1));

}

// Compute shape based interpolation volumes && interpolate MRI volumes
void shapeBasedInterpolationPrivate::computeShapeBasedInterpolation(int inputNumber)
{
    cout<<"shapeBasedInterpolationPrivate::computeShapeBasedInterpolation"<<endl;

    itk::MultiThreader::SetGlobalDefaultNumberOfThreads(8);

    int sizeZ = this->inputSize[inputNumber][2];

    // Create iterators on interpolated volumes
    MaskIterator itVolumOut(this->connectedComponent[inputNumber],
                            this->connectedComponent[inputNumber]->GetBufferedRegion() ); //Create image iterator
    itVolumOut.GoToBegin();

    MaskIterator itVolMRIOut(this->interpolatedMRI[inputNumber],
                             this->interpolatedMRI[inputNumber]->GetBufferedRegion() ); //Create image iterator
    itVolMRIOut.GoToBegin();

    MaskType::Pointer        volLabeled        = MaskType::New();
    Mask2dType::Pointer      img0              = Mask2dType::New();
    Mask2dType::Pointer      img1              = Mask2dType::New();
    Mask2dType::Pointer      mri0              = Mask2dType::New();
    Mask2dType::Pointer      mri1              = Mask2dType::New();
    Mask2dFloatType::Pointer distanceMapImg0   = Mask2dFloatType::New();
    Mask2dFloatType::Pointer distanceMapImg1   = Mask2dFloatType::New();

    // Connected components
    ConnectedComponentImageFilterType::Pointer labelFilter = ConnectedComponentImageFilterType::New(); // 3d
    labelFilter->SetFullyConnected(true);
    labelFilter->SetInput(this->input[inputNumber]);

    // Relabeling (Sort connected components)
    RelabelComponentImageFilterType::Pointer relabeller = RelabelComponentImageFilterType::New();
    relabeller->SetInput(labelFilter->GetOutput());
    relabeller->Update();

    BinaryThresholdImageFilterType::Pointer thresholdFilter  = BinaryThresholdImageFilterType::New();

    unsigned long nObjects = relabeller->GetNumberOfObjects();
    double nbObjectProg    = 50 / (nObjects*(sizeZ-1)+0.0);
    unsigned short isD0=0, isD1=0;

    // extract connected component // for each objects 0->6
    for (unsigned long k = 0; k < nObjects; ++k)
    {
        // threshold volume in order to keep each label component
        thresholdFilter->SetInput(relabeller->GetOutput());
        thresholdFilter->SetLowerThreshold(k+1);
        thresholdFilter->SetUpperThreshold(k+1);
        thresholdFilter->SetInsideValue(0); // 0->data
        thresholdFilter->SetOutsideValue(255);
        thresholdFilter->Update();

        volLabeled = thresholdFilter->GetOutput();
        volLabeled->Allocate();

        MaskType::RegionType inputRegionVolLabeled = volLabeled->GetLargestPossibleRegion();
        MaskType::SizeType   sizeVolLabeled        = inputRegionVolLabeled.GetSize();
        MaskType::IndexType  startVolLabeled       = inputRegionVolLabeled.GetIndex();

        img1 = extract2DImageSlice(volLabeled, 2, 0, sizeVolLabeled, startVolLabeled);
        isD1 = isData(img1);

        // for each slices 0->42
        for (int i=0; i<(sizeZ-1); ++i)
        {
            // update progress bar. Finish at 80%
            emit progressed( ((k*(sizeZ-1) + i+1) * nbObjectProg + inputNumber*50)*0.8 );

            img0 = img1;
            isD0 = isD1;

            img1 = extract2DImageSlice(volLabeled, 2, i+1, sizeVolLabeled, startVolLabeled);
            isD1 = isData(img1);

            if (isD0 || isD1)  // if there is data in at least one of these images
            {
                distanceMapImg0 = computeDistanceMap(img0);
                distanceMapImg1 = computeDistanceMap(img1);

                // Interpolate the "j" intermediate slice (float) // float->unsigned char 0/255 and copy into output volume
                for (int j=0; j<this->getIntermediateSlicesNumber[inputNumber]; ++j) // for each intermediate slice
                    computeIntermediateSlice(distanceMapImg0, distanceMapImg1, i, j, inputNumber, itVolumOut);

                // Interpolate MRI volumes
                if (k == 0)
                {
                    mri0 = extract2DImageSlice(this->input[inputNumber+2], 2, i,   sizeVolLabeled, startVolLabeled);
                    mri1 = extract2DImageSlice(this->input[inputNumber+2], 2, i+1, sizeVolLabeled, startVolLabeled);

                    for (int j=0; j<this->getIntermediateSlicesNumber[inputNumber]; ++j) // for each intermediate slice
                        computeIntermediateMRISlice(mri0, mri1, i, j, inputNumber, itVolMRIOut);
                }
            }
        } // end for each slice
    } // end for nObjects
}

// Is there data to observe in the image ?
unsigned short shapeBasedInterpolationPrivate::isData(Mask2dType::Pointer input)
{
    Mask2dIterator it(input, input->GetBufferedRegion());

    for (it.GoToBegin(); !it.IsAtEnd(); ++it)
    {
        if (it.Get() == 0) //data
            return 1;
    }
    return 0;
}

// Extract a 2D slice from a 3D volume
Mask2dType::Pointer shapeBasedInterpolationPrivate::extract2DImageSlice(MaskType::Pointer input,
                                                                        int plane,
                                                                        int slice,
                                                                        MaskType::SizeType size,
                                                                        MaskType::IndexType start
                                                                        )
{
    size[plane] = 0;
    const unsigned int sliceNumber = slice;
    start[plane] = sliceNumber;

    // create the desired region
    MaskType::RegionType desiredRegion;
    desiredRegion.SetSize(size);
    desiredRegion.SetIndex(start);

    // associate previous sequence and desired region
    Extract2DType::Pointer filter = Extract2DType::New();
    filter->SetExtractionRegion(desiredRegion);
    filter->SetInput(input);
    filter->SetDirectionCollapseToGuess();
    filter->Update();

    // extract the image
    return filter->GetOutput();
}

// Compute a distance map from a 2D image
Mask2dFloatType::Pointer shapeBasedInterpolationPrivate::computeDistanceMap(Mask2dType::Pointer img)
{
    LevelSetFilterType::Pointer levelSetFilter = LevelSetFilterType::New();
    levelSetFilter->SetInput(img);
    levelSetFilter->Update();

    DistanceMap2DType::Pointer distMapFilter = DistanceMap2DType::New();
    distMapFilter->SetInput(levelSetFilter->GetOutput());
    distMapFilter->SetInsideValue(0);
    distMapFilter->SetOutsideValue(255);
    distMapFilter->Update();

    return distMapFilter->GetOutput();
}

// Compute the interpolated slice between two distance maps
void shapeBasedInterpolationPrivate::computeIntermediateSlice(Mask2dFloatType::Pointer distanceMapImg0,
                                                              Mask2dFloatType::Pointer distanceMapImg1,
                                                              int i,
                                                              int j,
                                                              int inputNumber,
                                                              MaskIterator ito)
{
    // iterators
    Mask2dFloatIterator iti0(distanceMapImg0, distanceMapImg0->GetBufferedRegion()); // volume in 0
    iti0.GoToBegin();

    Mask2dFloatIterator iti1(distanceMapImg1, distanceMapImg1->GetBufferedRegion()); // volume in 1
    iti1.GoToBegin();

    // In order to copy the connected component map distance of this slice into the volume
    MaskType::IndexType start;
    start[0] = 0;
    start[1] = 0;
    start[2] = (i*(this->nbZIsotropicSlices[inputNumber]-1)) / (inputSize[inputNumber][2] - 1) + j; // i in the new volume // OK
    ito.SetIndex(start);

    // For each pixel of the InterpolatedSlice image, compute the value
    float interpolatVal;
    while(!iti0.IsAtEnd())
    {
        interpolatVal = (iti1.Get()-iti0.Get()) *
                    ((float)j/(float)this->getIntermediateSlicesNumber[inputNumber])
                    + iti0.Get();
        
        if (interpolatVal < 0.0f)
           ito.Set(255);

        ++iti0;
        ++iti1;
        ++ito;
    }
}

// Compute interpolated slice between two MRI slices
void shapeBasedInterpolationPrivate::computeIntermediateMRISlice(Mask2dType::Pointer mri0,
                                                              Mask2dType::Pointer mri1,
                                                              int i,
                                                              int j,
                                                              int inputNumber,
                                                              MaskIterator ito)
{
    // iterators
    Mask2dIterator iti0(mri0, mri0->GetBufferedRegion()); // volume in 0
    iti0.GoToBegin();

    Mask2dIterator iti1(mri1, mri1->GetBufferedRegion()); // volume in 1
    iti1.GoToBegin();

    // In order to copy the connected component map distance of this slice into the volume
    MaskType::IndexType start;
    start[0] = 0;
    start[1] = 0;
    start[2] = (i*(this->nbZIsotropicSlices[inputNumber]-1)) / (inputSize[inputNumber][2] - 1) + j; // i in the new volume // OK
    ito.SetIndex(start);

    // For each pixel of the InterpolatedSlice image, compute the value
    float interpolatVal;
    while(!iti0.IsAtEnd())
    {
        interpolatVal = (iti1.Get()-iti0.Get()) *
                    ((float)j/(float)this->getIntermediateSlicesNumber[inputNumber])
                    + iti0.Get();
        ito.Set(interpolatVal);

        ++iti0;
        ++iti1;
        ++ito;
    }
}

// Compute Distance Map, Gradient & Normalization on a 3D volume
void shapeBasedInterpolationPrivate::computeGradientMap(int inputNumber)
{
    // A'->A'3d and B'->B'3d doing 3D-distance maps on volumes A' and B'
    typedef itk::SignedMaurerDistanceMapImageFilter < MaskType, MaskFloatType > DistanceMap3DType;
    DistanceMap3DType::Pointer distanceMap3DFilter = DistanceMap3DType::New();
    distanceMap3DFilter->SetInput(this->connectedComponent[inputNumber]);
    distanceMap3DFilter->SetInsideIsPositive(true);
    distanceMap3DFilter->Update();

    // A'3d->GrA' and B'3d->GrB' doing a 3D-Gradient on these 3D-distance maps.
    typedef itk::DerivativeImageFilter<MaskFloatType, MaskFloatType >  GradientFilterType;
    GradientFilterType::Pointer gradientFilterX = GradientFilterType::New();
    gradientFilterX->SetInput( distanceMap3DFilter->GetOutput() );
    gradientFilterX->SetDirection(0); // "x" axis
    gradientFilterX->Update();

    GradientFilterType::Pointer gradientFilterY = GradientFilterType::New();
    gradientFilterY->SetInput( distanceMap3DFilter->GetOutput() );
    gradientFilterY->SetDirection(1); // "y" axis
    gradientFilterY->Update();

    // GrA'->GA' and GrB'->GB' normalizing these 3D-Gradient maps.
    typedef itk::NormalizeImageFilter< MaskFloatType, MaskFloatType >   NormalizeFilterType;
    NormalizeFilterType::Pointer normalizeFilterX = NormalizeFilterType::New();
    normalizeFilterX->SetInput(gradientFilterX->GetOutput());
    normalizeFilterX->Update();

    NormalizeFilterType::Pointer normalizeFilterY = NormalizeFilterType::New();
    normalizeFilterY->SetInput(gradientFilterY->GetOutput());
    normalizeFilterY->Update();

    this->gradientMap.push_back(normalizeFilterX->GetOutput());
    this->gradientMap.push_back(normalizeFilterY->GetOutput());

}

// Create empty final output, and fill it
void shapeBasedInterpolationPrivate::createFinalSeq()
{
    cout<<"shapeBasedInterpolationPrivate::createFinalSeq"<<endl;

    // -- Create output volume and fill with 0
    this->output = MaskType::New();

    this->output->SetRegions  (this->connectedComponent[0]->GetBufferedRegion());
    this->output->SetSpacing  (this->connectedComponent[0]->GetSpacing());
    this->output->SetOrigin   (this->connectedComponent[0]->GetOrigin());
    this->output->SetDirection(this->connectedComponent[0]->GetDirection());
    this->output->Allocate();
    this->output->FillBuffer(0);

    int shiftingZ = abs((int)((this->inputOrigin[0][0] - this->inputOrigin[1][0]) / this->inputSpacing[0][1]));
    int shiftingY = abs((int)((this->inputOrigin[0][2] - this->inputOrigin[1][2]) / this->inputSpacing[0][1]));

    MaskIterator it(this->output, this->output->GetBufferedRegion());

    for (it.GoToBegin(); !it.IsAtEnd(); ++it)
        this->computeFinalPix(it, shiftingZ, shiftingY);
}

// Compute an output pixel from computed volumes
void shapeBasedInterpolationPrivate::computeFinalPix(MaskIterator it, int shiftingZ, int shiftingY)
{
    // Get coordinates of pixel in output volume
    int x = it.GetIndex()[0];
    int y = it.GetIndex()[1];
    int z = it.GetIndex()[2];

    // Compute indexes of pixel in the second volume
    int indexZInVol1 = shiftingZ - z;
    int indexYInVol1 = shiftingY - y;

    // pixel index of mri volumes
    MaskType::IndexType indexPixelSeq1;
    indexPixelSeq1[0] = indexZInVol1;
    indexPixelSeq1[1] = x;
    indexPixelSeq1[2] = indexYInVol1;

    // Check current pixel in SECOND SEQ
    if (       (indexPixelSeq1[0] < 0)
            || (indexPixelSeq1[0] >=  (unsigned int)this->inputSize[1][0])
            || (indexPixelSeq1[1] < 0)
            || (indexPixelSeq1[1] >=  (unsigned int)this->inputSize[1][1])
            || (indexPixelSeq1[2] < 0)
            || (indexPixelSeq1[2] >=  this->nbZIsotropicSlices[1])     )
        return; // out of the sequence

    // pixel index of mri volumes
    MaskType::IndexType indexPixelSeq0;
    indexPixelSeq0[0] = x;
    indexPixelSeq0[1] = y;
    indexPixelSeq0[2] = z;

    if ( (this->connectedComponent[0]->GetPixel(indexPixelSeq0) > 0) || (this->connectedComponent[1]->GetPixel(indexPixelSeq1) > 0) ) //roi //if data
    {
        // pixel index of gradients
        MaskFloatType::IndexType indexPixelFSeq0;
        indexPixelFSeq0[0] = (float) x;
        indexPixelFSeq0[1] = (float) y;
        indexPixelFSeq0[2] = (float) z;

        MaskFloatType::IndexType indexPixelFSeq1;
        indexPixelFSeq1[0] = (float) indexZInVol1;
        indexPixelFSeq1[1] = (float) x;
        indexPixelFSeq1[2] = (float) indexYInVol1;

        //## Normalized Gradients of Distance Map
        float gradX0 = this->gradientMap[0]->GetPixel(indexPixelFSeq0);
        float gradY0 = this->gradientMap[1]->GetPixel(indexPixelFSeq0);
        float gradX1 = this->gradientMap[2]->GetPixel(indexPixelFSeq1);
        float gradY1 = this->gradientMap[3]->GetPixel(indexPixelFSeq1);

        //float mri0 = this->connectedComponent[0]->GetPixel(indexPixelSeq0);
        //float mri1 = this->connectedComponent[1]->GetPixel(indexPixelSeq1);
        float mri0 = this->interpolatedMRI[0]->GetPixel(indexPixelSeq0);
        float mri1 = this->interpolatedMRI[1]->GetPixel(indexPixelSeq1);

        //(1-<GA',OA>)C' + (1-<GB',OB>)D’ / (2 - <GA',OA> - <GB',OB>)
        const double OAx = this->connectedComponent[0]->GetDirection()[2][0];
        const double OAy = this->connectedComponent[0]->GetDirection()[2][1];
        const double OBx = this->connectedComponent[1]->GetDirection()[2][0];
        const double OBy = this->connectedComponent[1]->GetDirection()[2][1];

        double scalA  = gradX0 * OAx + gradY0 * OAy; //[gradX0,gradY0].[OAx,OAy]
        double scalB  = gradX1 * OBx + gradY1 * OBy; //[gradX1,gradY1].[OBx,OBy]

        if ( (2.0f - scalA - scalB) == 0.0f ) return;

        double magic = ((1.0f - scalA)*mri0 + (1.0f - scalB)*mri1) / (2.0f - scalA - scalB);

        this->output->SetPixel( indexPixelSeq0, magic);
    }
}

shapeBasedInterpolation::shapeBasedInterpolation(): d(new shapeBasedInterpolationPrivate)
{
    d->output = NULL;

    connect(d, SIGNAL(progressed(int)), this, SIGNAL(progressed(int)));
}

// destructor
shapeBasedInterpolation::~shapeBasedInterpolation()
{
    delete d;
    d = NULL;
}

void shapeBasedInterpolation::setInput(MaskType::Pointer input)
{
    if (input)
    {
        d->input.push_back(input);
    }
}

// return output
MaskType::Pointer shapeBasedInterpolation::output()
{
    return d->output;
}

// Run the algorithm from superResolutionProcess.cpp -> runShapeBasedInterpolation()
void shapeBasedInterpolation::run()
{
    d->createEmptyVolumes(0);
    d->createEmptyVolumes(1);

    // A->A' and B->B' doing distance map & interpolating ("shape-based interpolation")
    // A' and B' are isotropic and binaries.
    // In addition C and D (MRI volumes) are interpolated to be isotropic.
    d->computeShapeBasedInterpolation(0);
    d->computeShapeBasedInterpolation(1);
    if ( d->connectedComponent.empty() )
        return;

    // A'->A'3d and B'->B'3d doing 3D-distance maps on volumes A' and B'
    // A'3d->GrA' and B'3d->GrB' doing a 3D-Gradient on these 3D-distance maps.
    // GrA'->GA' and GrB'->GB' normalizing these 3D-Gradient maps.
    d->computeGradientMap(0);
    d->computeGradientMap(1);
    if ( d->gradientMap.empty() )
        return;

    // Compute the final sequence volume
    d->createFinalSeq();

    emit progressed(100);

    return;
}
