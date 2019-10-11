#include <vtkMatrix4x4.h>
#include <vtkMatrixToLinearTransform.h>
#include <vtkMetaDataSet.h>
#include <vtkPointSet.h>
#include <vtkSmartPointer.h>
#include <vtkTransformFilter.h>

#pragma once

class medAbstractData;
class vtkMatrix4x4;
class vtkMetaDataSet;
class vtkTransform;

namespace medTransform
{

void convert(const QMatrix4x4& source, vtkMatrix4x4& target);
void convert(const vtkMatrix4x4& source, QMatrix4x4& target);


// Functions using vtkMatrix4x4

void getTransform(itk::Object& object, vtkMatrix4x4& transform);
template <typename DATA_TYPE>
void getTransform(itk::ImageBase<3>& imageBase, vtkMatrix4x4& transform);

void setTransform(itk::Object& object, const vtkMatrix4x4& transform);
template <typename DATA_TYPE>
void setTransform(itk::ImageBase<3>& imageBase, const vtkMatrix4x4& transform);

void applyTransform(medAbstractData& data, const vtkMatrix4x4& transform);
void applyTransform(itk::Object& object, const vtkMatrix4x4& transform);
template <typename DATA_TYPE>
void applyTransform(itk::ImageBase<3>& imageBase, const vtkMatrix4x4& transform);
void applyTransform(vtkMetaDataSet& mesh, const vtkMatrix4x4& transform);


// Functions using QMatrix4x4

void getTransform(itk::Object& object, QMatrix4x4& transform);
template <typename DATA_TYPE>
void getTransform(itk::ImageBase<3>& object, QMatrix4x4& transform);

void setTransform(itk::Object& object, const QMatrix4x4& transform);
template <typename DATA_TYPE>
void setTransform(itk::ImageBase<3>&, const QMatrix4x4& transform);

void applyTransform(medAbstractData& data, const QMatrix4x4& transform);
void applyTransform(itk::Object& object, const QMatrix4x4& transform);
template <typename DATA_TYPE>
void applyTransform(itk::ImageBase<3>& imageBase, const QMatrix4x4& transform);
void applyTransform(vtkMetaDataSet& mesh, const QMatrix4x4& transform);


void convert(const QMatrix4x4& source, vtkMatrix4x4& target)
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            target.SetElement(i, j, source(i, j));
        }
    }
}

void convert(const vtkMatrix4x4& source, QMatrix4x4& target)
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            target(i, j) = source.GetElement(i, j);
        }
    }
}

vtkSmartPointer<vtkMatrix4x4> toVTKMatrix(const QMatrix4x4& matrix)
{
    vtkSmartPointer<vtkMatrix4x4> vtkMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    convert(matrix, *vtkMatrix);
    return vtkMatrix;
}

template <typename DATA_TYPE>
void getTransform(itk::ImageBase<3>& imageBase, vtkMatrix4x4& transform)
{
    typedef itk::Image<DATA_TYPE, 3> ImageType;
    ImageType& image = static_cast<ImageType&>(imageBase);
    typename ImageType::DirectionType orientation;
    typename ImageType::PointType origin;

    orientation = image.GetDirection();
    origin = image.GetOrigin();

    transform.Identity();
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            transform.SetElement(i, j, orientation(i, j));
        }
        transform.SetElement(i, 3, origin[i]);
    }
}

void getTransform(itk::Object& object, vtkMatrix4x4& transform)
{
    itk::ImageBase<3>& imageBase = dynamic_cast<itk::ImageBase<3>&>(object);

    if (dynamic_cast<itk::Image<char, 3>*>(&imageBase))
    {
        return getTransform<char>(imageBase, transform);
    }

    if (dynamic_cast<itk::Image<unsigned char, 3>*>(&imageBase))
    {
        return getTransform<unsigned char>(imageBase, transform);
    }

    if (dynamic_cast<itk::Image<short, 3>*>(&imageBase))
    {
        return getTransform<short>(imageBase, transform);
    }

    if (dynamic_cast<itk::Image<unsigned short, 3>*>(&imageBase))
    {
        return getTransform<unsigned short>(imageBase, transform);
    }

    if (dynamic_cast<itk::Image<int, 3>*>(&imageBase))
    {
        return getTransform<int>(imageBase, transform);
    }

    if (dynamic_cast<itk::Image<unsigned int, 3>*>(&imageBase))
    {
        return getTransform<unsigned int>(imageBase, transform);
    }

    if (dynamic_cast<itk::Image<long, 3>*>(&imageBase))
    {
        return getTransform<long>(imageBase, transform);
    }

    if (dynamic_cast<itk::Image<unsigned long, 3>*>(&imageBase))
    {
        return getTransform<unsigned long>(imageBase, transform);
    }

    if (dynamic_cast<itk::Image<float, 3>*>(&imageBase))
    {
        return getTransform<float>(imageBase, transform);
    }

    if (dynamic_cast<itk::Image<double, 3>*>(&imageBase))
    {
        return getTransform<double>(imageBase, transform);
    }
}

template <typename DATA_TYPE>
void setTransform(itk::ImageBase<3>& imageBase, const vtkMatrix4x4& transform)
{
    typedef itk::Image<DATA_TYPE, 3> ImageType;
    ImageType& image = static_cast<ImageType&>(imageBase);
    typename ImageType::DirectionType orientation;
    typename ImageType::PointType origin;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            orientation(i, j) = transform.GetElement(i, j);
        }
        origin[i] = transform.GetElement(i, 3);
    }

    image.SetDirection(orientation);
    image.SetOrigin(origin);
}

void setTransform(itk::Object& object, const vtkMatrix4x4& transform)
{
    itk::ImageBase<3>& imageBase = dynamic_cast<itk::ImageBase<3>&>(object);

    if (dynamic_cast<itk::Image<char, 3>*>(&imageBase))
    {
        return setTransform<char>(imageBase, transform);
    }

    if (dynamic_cast<itk::Image<unsigned char, 3>*>(&imageBase))
    {
        return setTransform<unsigned char>(imageBase, transform);
    }

    if (dynamic_cast<itk::Image<short, 3>*>(&imageBase))
    {
        return setTransform<short>(imageBase, transform);
    }

    if (dynamic_cast<itk::Image<unsigned short, 3>*>(&imageBase))
    {
        return setTransform<unsigned short>(imageBase, transform);
    }

    if (dynamic_cast<itk::Image<int, 3>*>(&imageBase))
    {
        return setTransform<int>(imageBase, transform);
    }

    if (dynamic_cast<itk::Image<unsigned int, 3>*>(&imageBase))
    {
        return setTransform<unsigned int>(imageBase, transform);
    }

    if (dynamic_cast<itk::Image<long, 3>*>(&imageBase))
    {
        return setTransform<long>(imageBase, transform);
    }

    if (dynamic_cast<itk::Image<unsigned long, 3>*>(&imageBase))
    {
        return setTransform<unsigned long>(imageBase, transform);
    }

    if (dynamic_cast<itk::Image<float, 3>*>(&imageBase))
    {
        return setTransform<float>(imageBase, transform);
    }

    if (dynamic_cast<itk::Image<double, 3>*>(&imageBase))
    {
        return setTransform<double>(imageBase, transform);
    }
}

template <typename DATA_TYPE>
void applyTransform(itk::ImageBase<3>& imageBase, const vtkMatrix4x4& transform)
{
    typedef itk::Image<DATA_TYPE, 3> ImageType;
    ImageType& image = static_cast<ImageType&>(imageBase);
    vtkSmartPointer<vtkMatrix4x4> imageTransform = vtkSmartPointer<vtkMatrix4x4>::New();

    getTransform<DATA_TYPE>(image, *imageTransform);
    vtkMatrix4x4::Multiply4x4(&transform, imageTransform, imageTransform);
    setTransform<DATA_TYPE>(image, *imageTransform);
}

void applyTransform(vtkMetaDataSet& mesh, const vtkMatrix4x4& transform)
{
    vtkSmartPointer<vtkMatrix4x4> transformCopy = vtkSmartPointer<vtkMatrix4x4>::New();
    transformCopy->DeepCopy(&transform);
    vtkSmartPointer<vtkMatrixToLinearTransform> matrixToTransform = vtkSmartPointer<vtkMatrixToLinearTransform>::New();
    matrixToTransform->SetInput(transformCopy);

    vtkSmartPointer<vtkTransformFilter> transformFilter = vtkSmartPointer<vtkTransformFilter>::New();
    transformFilter->SetInputData(mesh.GetDataSet());
    transformFilter->SetTransform(matrixToTransform);
    transformFilter->Update();

    mesh.SetDataSet(transformFilter->GetOutput());
}

void applyTransform(itk::Object& object, const vtkMatrix4x4& transform)
{
    itk::ImageBase<3>& imageBase = dynamic_cast<itk::ImageBase<3>&>(object);

    if (dynamic_cast<itk::Image<char, 3>*>(&imageBase))
    {
        return applyTransform<char>(imageBase, transform);
    }

    if (dynamic_cast<itk::Image<unsigned char, 3>*>(&imageBase))
    {
        return applyTransform<unsigned char>(imageBase, transform);
    }

    if (dynamic_cast<itk::Image<short, 3>*>(&imageBase))
    {
        return applyTransform<short>(imageBase, transform);
    }

    if (dynamic_cast<itk::Image<unsigned short, 3>*>(&imageBase))
    {
        return applyTransform<unsigned short>(imageBase, transform);
    }

    if (dynamic_cast<itk::Image<int, 3>*>(&imageBase))
    {
        return applyTransform<int>(imageBase, transform);
    }

    if (dynamic_cast<itk::Image<unsigned int, 3>*>(&imageBase))
    {
        return applyTransform<unsigned int>(imageBase, transform);
    }

    if (dynamic_cast<itk::Image<long, 3>*>(&imageBase))
    {
        return applyTransform<long>(imageBase, transform);
    }

    if (dynamic_cast<itk::Image<unsigned long, 3>*>(&imageBase))
    {
        return applyTransform<unsigned long>(imageBase, transform);
    }

    if (dynamic_cast<itk::Image<float, 3>*>(&imageBase))
    {
        return applyTransform<float>(imageBase, transform);
    }

    if (dynamic_cast<itk::Image<double, 3>*>(&imageBase))
    {
        return applyTransform<double>(imageBase, transform);
    }
}

void applyTransform(medAbstractData& data, const vtkMatrix4x4& transform)
{
    QString identifier = data.identifier();

    if (identifier.startsWith("itkDataImage"))
    {
        return applyTransform(*static_cast<itk::Object*>(data.data()), transform);
    }
    else if (identifier.startsWith("vtkDataMesh"))
    {
        return applyTransform(*vtkMetaDataSet::SafeDownCast(static_cast<vtkObject*>(data.data())), transform);
    }
}

void getTransform(itk::Object& object, QMatrix4x4& transform)
{
    vtkSmartPointer<vtkMatrix4x4> vtkMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    getTransform(object, *vtkMatrix);
    convert(*vtkMatrix, transform);
}

template <typename DATA_TYPE>
void getTransform(itk::ImageBase<3>& object, QMatrix4x4& transform)
{
    vtkSmartPointer<vtkMatrix4x4> vtkMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    getTransform<DATA_TYPE>(object, vtkMatrix);
    convert(*vtkMatrix, transform);
}

void setTransform(itk::Object& object, const QMatrix4x4& transform)
{
    setTransform(dynamic_cast<itk::ImageBase<3>&>(object), *toVTKMatrix(transform));
}

template <typename DATA_TYPE>
void setTransform(itk::ImageBase<3>& imageBase, const QMatrix4x4& transform)
{
    setTransform<DATA_TYPE>(imageBase, *toVTKMatrix(transform));
}

void applyTransform(medAbstractData& data, const QMatrix4x4& transform)
{
    applyTransform(data, *toVTKMatrix(transform));
}

void applyTransform(itk::Object& object, const QMatrix4x4& transform)
{
    applyTransform(object, *toVTKMatrix(transform));
}

template <typename DATA_TYPE>
void applyTransform(itk::ImageBase<3>& imageBase, const QMatrix4x4 transform)
{
    applyTransform<DATA_TYPE>(imageBase, *toVTKMatrix(transform));
}

void applyTransform(vtkMetaDataSet& mesh, const QMatrix4x4& transform)
{
    applyTransform(mesh, *toVTKMatrix(transform));
}

}
