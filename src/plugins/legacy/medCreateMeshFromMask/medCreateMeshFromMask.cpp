/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <dtkCoreSupport/dtkAbstractProcessFactory.h>
#include <dtkCoreSupport/dtkSmartPointer.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>
#include <medCreateMeshFromMask.h>
#include <medUtilities.h>

#include <itkImage.h>
#include <itkImageToVTKImageFilter.h>

#include <vtkContourFilter.h>
#include <vtkDecimatePro.h>
#include <vtkMetaSurfaceMesh.h>
#include <vtkSmartPointer.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTriangleFilter.h>

// /////////////////////////////////////////////////////////////////
// medCreateMeshFromMaskPrivate
// /////////////////////////////////////////////////////////////////

class medCreateMeshFromMaskPrivate
{
public:
    dtkSmartPointer <medAbstractData> input;
    dtkSmartPointer <medAbstractData> output;
    double isoValue;
    double targetReduction;
    bool decimate;
    bool smooth;
    int iterations;
    double relaxationFactor;
    int nbTriangles;

    template <class PixelType> int update();
};

template <class PixelType> int medCreateMeshFromMaskPrivate::update()
{
    typedef itk::Image<PixelType, 3> ImageType;
    typedef itk::ImageToVTKImageFilter<ImageType>  FilterType;
    typename FilterType::Pointer filter = FilterType::New();

    typename ImageType::Pointer img = static_cast<ImageType *>(input->data());
    filter->SetInput(img);
    filter->Update();

    // ----- Hack to keep the itkImages info (origin and orientation)
    vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
    matrix->Identity();
    for (unsigned int x=0; x<3; x++)
    {
        for (unsigned int y=0; y<3; y++)
        {
            matrix->SetElement(x, y, img->GetDirection()[x][y]);
        }
    }

    typename itk::ImageBase<3>::PointType origin = img->GetOrigin();
    double v_origin[4], v_origin2[4];
    for (int i=0; i<3; i++)
    {
        v_origin[i] = origin[i];
    }
    v_origin[3] = 1.0;
    matrix->MultiplyPoint(v_origin, v_origin2);
    for (int i=0; i<3; i++)
    {
        matrix->SetElement(i, 3, v_origin[i]-v_origin2[i]);
    }

    //------------------------------------------------------

    vtkImageData *vtkImage = filter->GetOutput();

    vtkContourFilter *contour = vtkContourFilter::New();
    contour->SetInputData(vtkImage);
    contour->SetValue(0, isoValue);
    contour->Update();

    vtkTriangleFilter *contourTrian = vtkTriangleFilter::New();
    contourTrian->SetInputConnection(contour->GetOutputPort());
    contourTrian->PassVertsOn();
    contourTrian->PassLinesOn();
    contourTrian->Update();

    vtkPolyDataAlgorithm *lastAlgo = contourTrian;

    vtkDecimatePro *contourDecimated = nullptr;
    if (decimate)
    {
        // Decimate the mesh if required
        contourDecimated = vtkDecimatePro::New();
        contourDecimated->SetInputConnection(lastAlgo->GetOutputPort());
        contourDecimated->SetTargetReduction(targetReduction);
        contourDecimated->SplittingOff();
        contourDecimated->PreserveTopologyOn();
        contourDecimated->Update();
        lastAlgo = contourDecimated;
    }

    vtkSmoothPolyDataFilter *contourSmoothed = nullptr;
    if(smooth)
    {
        // Smooth the mesh if required
        contourSmoothed = vtkSmoothPolyDataFilter::New();
        contourSmoothed->SetInputConnection(lastAlgo->GetOutputPort());
        contourSmoothed->SetNumberOfIterations(iterations);
        contourSmoothed->SetRelaxationFactor(relaxationFactor);
        contourSmoothed->Update();
        lastAlgo = contourSmoothed;
    }

    vtkPolyData *polydata = lastAlgo->GetOutput();
    nbTriangles = polydata->GetNumberOfPolys();

    if (nbTriangles > 0)
    {
        // To get the itkImage info back
        vtkSmartPointer<vtkTransform> t = vtkSmartPointer<vtkTransform>::New();
        t->SetMatrix(matrix);

        vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
        transformFilter->SetInputConnection(lastAlgo->GetOutputPort());
        transformFilter->SetTransform(t);
        transformFilter->Update();

        polydata->DeepCopy(transformFilter->GetOutput());

        vtkMetaSurfaceMesh *smesh = vtkMetaSurfaceMesh::New();
        smesh->SetDataSet(polydata);

        contour->Delete();
        contourTrian->Delete();
        if (contourDecimated)
        {
            contourDecimated->Delete();
        }
        if (contourSmoothed)
        {
            contourSmoothed->Delete();
        }

        output = medAbstractDataFactory::instance()->createSmartPointer("vtkDataMesh");
        medUtilities::setDerivedMetaData(output, input, "mesh from mask");
        output->setData(smesh);

        return medAbstractProcessLegacy::SUCCESS;
    }

    output = nullptr;
    return medAbstractProcessLegacy::FAILURE;
}

// /////////////////////////////////////////////////////////////////
// medCreateMeshFromMask
// /////////////////////////////////////////////////////////////////

medCreateMeshFromMask::medCreateMeshFromMask() : medAbstractProcessLegacy(), d(new medCreateMeshFromMaskPrivate)
{
    d->output = nullptr;
}

medCreateMeshFromMask::~medCreateMeshFromMask()
{
    delete d;
    d = nullptr;
}

bool medCreateMeshFromMask::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("medCreateMeshFromMask", createmedCreateMeshFromMask);
}

QString medCreateMeshFromMask::description() const
{
    return "medCreateMeshFromMask";
}

void medCreateMeshFromMask::setInput ( medAbstractData *data, int channel )
{
    d->input = data;
}

void medCreateMeshFromMask::setParameter ( double data, int channel )
{
    switch (channel)
    {
        case 0:
            d->isoValue = data;
            break;
        case 1:
            d->decimate = (data > 0) ? true : false;
            break;
        case 2:
            d->targetReduction = data;
            break;
        case 3:
            d->smooth = (data > 0) ? true : false;
            break;
        case 4:
            d->iterations = data;
            break;
        case 5:
            d->relaxationFactor = data;
            break;
    }
}

int medCreateMeshFromMask::update()
{
    int res = medAbstractProcessLegacy::FAILURE;

    if ( d->input )
    {
        const QString& id = d->input->identifier();

        if (id == "itkDataImageChar3")
        {
            res = d->update<char>();
        }
        else if (id == "itkDataImageUChar3")
        {
            res = d->update<unsigned char>();
        }
        else if (id == "itkDataImageShort3")
        {
            res = d->update<short>();
        }
        else if (id == "itkDataImageUShort3")
        {
            res = d->update<unsigned short>();
        }
        else if (id == "itkDataImageInt3")
        {
            res = d->update<int>();
        }
        else if (id == "itkDataImageUInt3")
        {
            res = d->update<unsigned int>();
        }
        else if (id == "itkDataImageLong3")
        {
            res = d->update<long>();
        }
        else if (id== "itkDataImageULong3")
        {
            res = d->update<unsigned long>();
        }
        else if (id == "itkDataImageFloat3")
        {
            res = d->update<float>();
        }
        else if (id == "itkDataImageDouble3")
        {
            res = d->update<double>();
        }
        else
        {
            res = medAbstractProcessLegacy::PIXEL_TYPE;
        }
    }
    return res;
}

medAbstractData * medCreateMeshFromMask::output()
{
    return d->output;
}

int medCreateMeshFromMask::getNumberOfTriangles()
{
    return d->nbTriangles;
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createmedCreateMeshFromMask()
{
    return new medCreateMeshFromMask;
}
