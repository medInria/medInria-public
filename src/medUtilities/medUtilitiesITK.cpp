#include "medUtilitiesITK.h"

#include <statsROI.h>

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

template <typename ImageType>
QString medUtilitiesITK::itkDataImageId()
{
    return QString();
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
