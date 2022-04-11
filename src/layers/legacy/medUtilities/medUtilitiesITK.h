#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medUtilitiesExport.h"

#include <dtkCoreSupport/dtkSmartPointer>

#include <itkImage.h>

#include <medAbstractProcessLegacy.h>
#include <medMetaDataKeys.h>

class medAbstractData;

class MEDUTILITIES_EXPORT medUtilitiesITK
{
public:

    static dtkSmartPointer<medAbstractData> binarizeMask(dtkSmartPointer<medAbstractData> data);
    
    /**
     * @brief minimumValue computes the minimum pixel intensity in a volume
     * @param the input volume
     * @return the minimum intensity value (double)
     */
    static double minimumValue(dtkSmartPointer<medAbstractData> data);

    /**
     * @brief maximumValue computes the maximum pixel intensity in a volume
     * @param the input volume
     * @return the maximum intensity value (double)
     */
    static double maximumValue(dtkSmartPointer<medAbstractData> data);

    static double volume(dtkSmartPointer<medAbstractData> data);

    static void meanStdDeviation(dtkSmartPointer<medAbstractData> data,
                                dtkSmartPointer<medAbstractData> mask,
                                double *mean, double *stdDeviation);

    typedef itk::Image<char, 3> itkImageChar3;
    typedef itk::Image<unsigned char, 3> itkImageUChar3;
    typedef itk::Image<short, 3> itkImageShort3;
    typedef itk::Image<unsigned short, 3> itkImageUShort3;
    typedef itk::Image<int, 3> itkImageInt3;
    typedef itk::Image<unsigned int, 3> itkImageUInt3;
    typedef itk::Image<long, 3> itkImageLong3;
    typedef itk::Image<unsigned long, 3> itkImageULong3;
    typedef itk::Image<float, 3> itkImageFloat3;
    typedef itk::Image<double, 3> itkImageDouble3;
    typedef itk::Image<char, 4> itkImageChar4;
    typedef itk::Image<unsigned char, 4> itkImageUChar4;
    typedef itk::Image<short, 4> itkImageShort4;
    typedef itk::Image<unsigned short, 4> itkImageUShort4;
    typedef itk::Image<int, 4> itkImageInt4;
    typedef itk::Image<unsigned int, 4> itkImageUInt4;
    typedef itk::Image<long, 4> itkImageLong4;
    typedef itk::Image<unsigned long, 4> itkImageULong4;
    typedef itk::Image<float, 4> itkImageFloat4;
    typedef itk::Image<double, 4> itkImageDouble4;

    /** Identifier of the class used to store a specific itk image type, to be used with dtkAbstractDataFactory
     */
    template <typename ImageType> static QString itkDataImageId();
    static QString itkDataImageId(QString type, unsigned int dimension = 3);
#ifndef SWIG

    template <typename TargetClass, typename... OtherArguments>
    static int dispatchOn3DPixelType(int (TargetClass::*char3Function)(medAbstractData*, OtherArguments...),
                                     int (TargetClass::*uchar3Function)(medAbstractData*, OtherArguments...),
                                     int (TargetClass::*short3Function)(medAbstractData*, OtherArguments...),
                                     int (TargetClass::*ushort3Function)(medAbstractData*, OtherArguments...),
                                     int (TargetClass::*int3Function)(medAbstractData*, OtherArguments...),
                                     int (TargetClass::*uint3Function)(medAbstractData*, OtherArguments...),
                                     int (TargetClass::*long3Function)(medAbstractData*, OtherArguments...),
                                     int (TargetClass::*ulong3Function)(medAbstractData*, OtherArguments...),
                                     int (TargetClass::*float3Function)(medAbstractData*, OtherArguments...),
                                     int (TargetClass::*double3Function)(medAbstractData*, OtherArguments...),
                                     TargetClass* targetInstance,
                                     medAbstractData* inputData,
                                     OtherArguments... otherArgs)
    {
        int result;

        if (callPixelSpecificFunction<TargetClass, itkImageChar3, OtherArguments...>(char3Function, targetInstance, &result, inputData, otherArgs...)
            || callPixelSpecificFunction<TargetClass, itkImageUChar3, OtherArguments...>(uchar3Function, targetInstance, &result, inputData, otherArgs...)
            || callPixelSpecificFunction<TargetClass, itkImageShort3, OtherArguments...>(short3Function, targetInstance, &result, inputData, otherArgs...)
            || callPixelSpecificFunction<TargetClass, itkImageUShort3, OtherArguments...>(ushort3Function, targetInstance, &result, inputData, otherArgs...)
            || callPixelSpecificFunction<TargetClass, itkImageInt3, OtherArguments...>(int3Function, targetInstance, &result, inputData, otherArgs...)
            || callPixelSpecificFunction<TargetClass, itkImageUInt3, OtherArguments...>(uint3Function, targetInstance, &result, inputData, otherArgs...)
            || callPixelSpecificFunction<TargetClass, itkImageLong3, OtherArguments...>(long3Function, targetInstance, &result, inputData, otherArgs...)
            || callPixelSpecificFunction<TargetClass, itkImageULong3, OtherArguments...>(ulong3Function, targetInstance, &result, inputData, otherArgs...)
            || callPixelSpecificFunction<TargetClass, itkImageFloat3, OtherArguments...>(float3Function, targetInstance, &result, inputData, otherArgs...)
            || callPixelSpecificFunction<TargetClass, itkImageDouble3, OtherArguments...>(double3Function, targetInstance, &result, inputData, otherArgs...))
        {
            return result;
        }
        else
        {
            return medAbstractProcessLegacy::PIXEL_TYPE;
        }
    }

    template <typename TargetClass, typename... OtherArguments>
    static int dispatchOn4DPixelType(int (TargetClass::*char4Function)(medAbstractData*, OtherArguments...),
                                     int (TargetClass::*uchar4Function)(medAbstractData*, OtherArguments...),
                                     int (TargetClass::*short4Function)(medAbstractData*, OtherArguments...),
                                     int (TargetClass::*ushort4Function)(medAbstractData*, OtherArguments...),
                                     int (TargetClass::*int4Function)(medAbstractData*, OtherArguments...),
                                     int (TargetClass::*uint4Function)(medAbstractData*, OtherArguments...),
                                     int (TargetClass::*long4Function)(medAbstractData*, OtherArguments...),
                                     int (TargetClass::*ulong4Function)(medAbstractData*, OtherArguments...),
                                     int (TargetClass::*float4Function)(medAbstractData*, OtherArguments...),
                                     int (TargetClass::*double4Function)(medAbstractData*, OtherArguments...),
                                     TargetClass* targetInstance,
                                     medAbstractData* inputData,
                                     OtherArguments... otherArgs)
    {
        int result;

        if (callPixelSpecificFunction<TargetClass, itkImageChar4, OtherArguments...>(char4Function, targetInstance, &result, inputData, otherArgs...)
            || callPixelSpecificFunction<TargetClass, itkImageUChar4, OtherArguments...>(uchar4Function, targetInstance, &result, inputData, otherArgs...)
            || callPixelSpecificFunction<TargetClass, itkImageShort4, OtherArguments...>(short4Function, targetInstance, &result, inputData, otherArgs...)
            || callPixelSpecificFunction<TargetClass, itkImageUShort4, OtherArguments...>(ushort4Function, targetInstance, &result, inputData, otherArgs...)
            || callPixelSpecificFunction<TargetClass, itkImageInt4, OtherArguments...>(int4Function, targetInstance, &result, inputData, otherArgs...)
            || callPixelSpecificFunction<TargetClass, itkImageUInt4, OtherArguments...>(uint4Function, targetInstance, &result, inputData, otherArgs...)
            || callPixelSpecificFunction<TargetClass, itkImageLong4, OtherArguments...>(long4Function, targetInstance, &result, inputData, otherArgs...)
            || callPixelSpecificFunction<TargetClass, itkImageULong4, OtherArguments...>(ulong4Function, targetInstance, &result, inputData, otherArgs...)
            || callPixelSpecificFunction<TargetClass, itkImageFloat4, OtherArguments...>(float4Function, targetInstance, &result, inputData, otherArgs...)
            || callPixelSpecificFunction<TargetClass, itkImageDouble4, OtherArguments...>(double4Function, targetInstance, &result, inputData, otherArgs...))
        {
            return result;
        }
        else
        {
            return medAbstractProcessLegacy::PIXEL_TYPE;
        }
    }

    /**
     * Update metadata: Columns, Rows, Size, SliceThickness, Orientation and Origin.
     * XSpacing, YSpacing and ZSpacing are not filled in itkDCMTKDataImageReader.cpp
     * However, SliceThickness == ZSpacing, and X and Y spacings can be read by the user on the view.
     */
    template <typename ImageType> static void updateMetadata(medAbstractData* outputData)
    {
        typename ImageType::Pointer outputImage = static_cast<ImageType*>(outputData->data());

        outputData->setMetaData(medMetaDataKeys::Columns.key(),
                                QString::number(outputImage->GetLargestPossibleRegion().GetSize()[0]));
        outputData->setMetaData(medMetaDataKeys::Rows.key(),
                                QString::number(outputImage->GetLargestPossibleRegion().GetSize()[1]));
        outputData->setMetaData(medMetaDataKeys::Size.key(),
                                QString::number(outputImage->GetLargestPossibleRegion().GetSize()[2]));
        outputData->setMetaData(medMetaDataKeys::SliceThickness.key(),
                                QString::number(outputImage->GetSpacing()[2]));

        outputData->setMetaData(medMetaDataKeys::Orientation.key(),
                QString::number(outputImage->GetDirection()[0][0]) +
                QString(" ") +
                QString::number(outputImage->GetDirection()[0][1]) +
                QString(" ") +
                QString::number(outputImage->GetDirection()[0][2]) +
                QString(" ") +
                QString::number(outputImage->GetDirection()[1][0]) +
                QString(" ") +
                QString::number(outputImage->GetDirection()[1][1]) +
                QString(" ") +
                QString::number(outputImage->GetDirection()[1][2]));

        outputData->setMetaData(medMetaDataKeys::Origin.key(),
                QString::number(outputImage->GetOrigin()[0]) +
                QString(" ") +
                QString::number(outputImage->GetOrigin()[1]) +
                QString(" ") +
                QString::number(outputImage->GetOrigin()[2]));
    }

private:

    template <typename TargetClass, typename ImageType, typename... OtherArguments>
    static bool callPixelSpecificFunction(int (TargetClass::*function)(medAbstractData*, OtherArguments...),
                                          TargetClass* targetInstance, int* result, medAbstractData* inputData, OtherArguments... otherArgs)
    {
        if ((function != nullptr) && (inputData->identifier().startsWith("itkDataImage")))
        {
            ImageType* image = dynamic_cast<ImageType*>(static_cast<itk::ImageBase<ImageType::ImageDimension>*>(inputData->data()));

            if (image)
            {
                *result = (targetInstance->*function)(inputData, otherArgs...);
                return true;
            }
        }

        return false;
    }

#endif

};

#ifndef SWIG

/** Call a function templated over the pixel type of an ITK image
 *  This macro will only check for 3D pixel types. The function signature must be of the form
 *  @code
 *  template<ImageType> int function(medAbstractData*, ...)
 *  @endcode
 *  where ImageType is the ITK type, the first argument is the data containing the image and ... may be zero or more arguments of any type.
 *  @param function a pointer to a member function (see signature above)
 *  @param instance the instance on which to invoke the function
 *  @param ... the arguments to the function
 *  @return the return value of the function or medAbstractProcess::PIXEL_TYPE if the pixel type was not handled
 */
#define DISPATCH_ON_3D_PIXEL_TYPE(function, instance, ...) \
    medUtilitiesITK::dispatchOn3DPixelType( \
    function<medUtilitiesITK::itkImageChar3>, \
    function<medUtilitiesITK::itkImageUChar3>, \
    function<medUtilitiesITK::itkImageShort3>, \
    function<medUtilitiesITK::itkImageUShort3>, \
    function<medUtilitiesITK::itkImageInt3>, \
    function<medUtilitiesITK::itkImageUInt3>, \
    function<medUtilitiesITK::itkImageLong3>, \
    function<medUtilitiesITK::itkImageULong3>, \
    function<medUtilitiesITK::itkImageFloat3>, \
    function<medUtilitiesITK::itkImageDouble3>, \
    (instance), \
    __VA_ARGS__)


/** Call a function templated over the pixel type of an ITK image
 *  This macro will only check for 4D pixel types. The function signature must be of the form
 *  @code
 *  template<ImageType> int function(medAbstractData*, ...)
 *  @endcode
 *  where ImageType is the ITK type, the first argument is the data containing the image and ... may be zero or more arguments of any type.
 *  @param function a pointer to a member function (see signature above)
 *  @param instance the instance on which to invoke the function
 *  @param ... the arguments to the function
 *  @return the return value of the function or medAbstractProcess::PIXEL_TYPE if the pixel type was not handled
 */
#define DISPATCH_ON_4D_PIXEL_TYPE(function, instance, ...) \
    medUtilitiesITK::dispatchOn4DPixelType( \
    function<medUtilitiesITK::itkImageChar4>, \
    function<medUtilitiesITK::itkImageUChar4>, \
    function<medUtilitiesITK::itkImageShort4>, \
    function<medUtilitiesITK::itkImageUShort4>, \
    function<medUtilitiesITK::itkImageInt4>, \
    function<medUtilitiesITK::itkImageUInt4>, \
    function<medUtilitiesITK::itkImageLong4>, \
    function<medUtilitiesITK::itkImageULong4>, \
    function<medUtilitiesITK::itkImageFloat4>, \
    function<medUtilitiesITK::itkImageDouble4>, \
    (instance), \
    __VA_ARGS__)


template<> MEDUTILITIES_EXPORT QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageChar3>();
template<> MEDUTILITIES_EXPORT QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageChar4>();
template<> MEDUTILITIES_EXPORT QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageUChar3>();
template<> MEDUTILITIES_EXPORT QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageUChar4>();
template<> MEDUTILITIES_EXPORT QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageShort3>();
template<> MEDUTILITIES_EXPORT QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageShort4>();
template<> MEDUTILITIES_EXPORT QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageUShort3>();
template<> MEDUTILITIES_EXPORT QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageUShort4>();
template<> MEDUTILITIES_EXPORT QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageInt3>();
template<> MEDUTILITIES_EXPORT QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageInt4>();
template<> MEDUTILITIES_EXPORT QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageUInt3>();
template<> MEDUTILITIES_EXPORT QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageUInt4>();
template<> MEDUTILITIES_EXPORT QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageLong3>();
template<> MEDUTILITIES_EXPORT QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageLong4>();
template<> MEDUTILITIES_EXPORT QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageULong3>();
template<> MEDUTILITIES_EXPORT QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageULong4>();
template<> MEDUTILITIES_EXPORT QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageFloat3>();
template<> MEDUTILITIES_EXPORT QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageFloat4>();
template<> MEDUTILITIES_EXPORT QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageDouble3>();
template<> MEDUTILITIES_EXPORT QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageDouble4>();

#endif
