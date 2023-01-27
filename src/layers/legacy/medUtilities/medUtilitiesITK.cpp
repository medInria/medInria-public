/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include "medUtilitiesITK.h"

#include <imageFilters.h>
#include <statsROI.h>

/**
 * @brief For masks with non-0/1 values, as -1024/10000, set the intensity to 0/1
 *
 * @param data
 */
dtkSmartPointer<medAbstractData> medUtilitiesITK::binarizeMask(dtkSmartPointer<medAbstractData> data)
{
    dtkSmartPointer<medAbstractData> outputData;

    imageFilters process;
    process.setInput(data);
    process.setParameter(imageFilters::BINARIZE);
    if (process.update())
    {
        outputData = process.dataOutput();
    }
    return outputData;
}

double medUtilitiesITK::minimumValue(dtkSmartPointer<medAbstractData> data)
{
    statsROI statsProcess;
    statsProcess.setInput(data, 0);
    statsProcess.setParameter(statsROI::MINMAX);
    statsProcess.update();
    return statsProcess.output().at(0);
}

double medUtilitiesITK::maximumValue(dtkSmartPointer<medAbstractData> data)
{
    statsROI statsProcess;
    statsProcess.setInput(data, 0);
    statsProcess.setParameter(statsROI::MINMAX);
    statsProcess.update();
    return statsProcess.output().at(1);
}

double medUtilitiesITK::volume(dtkSmartPointer<medAbstractData> data)
{
    statsROI statsProcess;
    statsProcess.setInput(data, 0);
    statsProcess.setParameter(statsROI::VOLUMEML);
    statsProcess.update();
    return statsProcess.output().at(0);
}

void medUtilitiesITK::meanStdDeviation(dtkSmartPointer<medAbstractData> data,
                                      dtkSmartPointer<medAbstractData> mask,
                                      double *mean,
                                      double *stdDeviation)
{
    statsROI statsProcess;
    statsProcess.setInput(data, 0);
    statsProcess.setInput(mask, 1);
    statsProcess.setParameter(statsROI::MEAN_STDDEVIATION);
    statsProcess.update();
    *mean = statsProcess.output().at(0);
    *stdDeviation = statsProcess.output().at(1);
    return;
}

template <typename ImageType>
QString medUtilitiesITK::itkDataImageId()
{
    return QString();
}

QString medUtilitiesITK::itkDataImageId(QString type,  unsigned int dimension)
{
    if (dimension != 3 && dimension != 4)
    {
        return "";
    }

    if (type == "char")
    {
        if (dimension == 3)
        {
            return itkDataImageId<itk::Image<char, 3> >();
        }
        else if (dimension == 4)
        {
            return itkDataImageId<itk::Image<char, 4> >();
        }
    }
    else if (type == "unsigned char")
    {
        if (dimension == 3)
        {
            return itkDataImageId<itk::Image<unsigned char, 3> >();
        }
        else if (dimension == 4)
        {
            return itkDataImageId<itk::Image<unsigned char, 4> >();
        }
    }
    else if (type == "short")
    {
        if (dimension == 3)
        {
            return itkDataImageId<itk::Image<short, 3> >();
        }
        else if (dimension == 4)
        {
            return itkDataImageId<itk::Image<short, 4> >();
        }
    }
    else if (type == "unsigned short")
    {
        if (dimension == 3)
        {
            return itkDataImageId<itk::Image<unsigned short, 3> >();
        }
        else if (dimension == 4)
        {
            return itkDataImageId<itk::Image<unsigned short, 4> >();
        }
    }
    else if (type == "int")
    {
        if (dimension == 3)
        {
            return itkDataImageId<itk::Image<int, 3> >();
        }
        else if (dimension == 4)
        {
            return itkDataImageId<itk::Image<int, 4> >();
        }
    }
    else if (type == "unsigned int")
    {
        if (dimension == 3)
        {
            return itkDataImageId<itk::Image<unsigned int, 3> >();
        }
        else if (dimension == 4)
        {
            return itkDataImageId<itk::Image<unsigned int, 4> >();
        }
    }
    else if (type == "long")
    {
        if (dimension == 3)
        {
            return itkDataImageId<itk::Image<long, 3> >();
        }
        else if (dimension == 4)
        {
            return itkDataImageId<itk::Image<long, 4> >();
        }
    }
    else if (type == "unsigned long")
    {
        if (dimension == 3)
        {
            return itkDataImageId<itk::Image<unsigned long, 3> >();
        }
        else if (dimension == 4)
        {
            return itkDataImageId<itk::Image<unsigned long, 4> >();
        }
    }
    else if (type == "float")
    {
        if (dimension == 3)
        {
            return itkDataImageId<itk::Image<float, 3> >();
        }
        else if (dimension == 4)
        {
            return itkDataImageId<itk::Image<float, 4> >();
        }
    }
    else if (type == "double")
    {
        if (dimension == 3)
        {
            return itkDataImageId<itk::Image<double, 3> >();
        }
        else if (dimension == 4)
        {
            return itkDataImageId<itk::Image<double, 4> >();
        }
    }
    return "";
}

template <>
QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageChar3>()
{
    return "itkDataImageChar3";
}

template <>
QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageChar4>()
{
    return "itkDataImageChar4";
}

template <>
QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageUChar3>()
{
    return "itkDataImageUChar3";
}

template <>
QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageUChar4>()
{
    return "itkDataImageUChar4";
}

template <>
QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageShort3>()
{
    return "itkDataImageShort3";
}

template <>
QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageShort4>()
{
    return "itkDataImageShort4";
}

template <>
QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageUShort3>()
{
    return "itkDataImageUShort3";
}

template <>
QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageUShort4>()
{
    return "itkDataImageUShort4";
}

template <>
QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageInt3>()
{
    return "itkDataImageInt3";
}

template <>
QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageInt4>()
{
    return "itkDataImageInt4";
}

template <>
QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageUInt3>()
{
    return "itkDataImageUInt3";
}

template <>
QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageUInt4>()
{
    return "itkDataImageUInt4";
}

template <>
QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageLong3>()
{
    return "itkDataImageLong3";
}

template <>
QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageLong4>()
{
    return "itkDataImageLong4";
}

template <>
QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageULong3>()
{
    return "itkDataImageULong3";
}

template <>
QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageULong4>()
{
    return "itkDataImageULong4";
}

template <>
QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageFloat3>()
{
    return "itkDataImageFloat3";
}

template <>
QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageFloat4>()
{
    return "itkDataImageFloat4";
}

template <>
QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageDouble3>()
{
    return "itkDataImageDouble3";
}

template <>
QString medUtilitiesITK::itkDataImageId<medUtilitiesITK::itkImageDouble4>()
{
    return "itkDataImageDouble4";
}
