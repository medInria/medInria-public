/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <meshMapping.h>

#include <dtkCoreSupport/dtkAbstractProcess.h>
#include <dtkCoreSupport/dtkAbstractProcessFactory.h>
#include <dtkCoreSupport/dtkSmartPointer.h>

#include <medAbstractDataFactory.h>
#include <medMetaDataKeys.h>
#include <medUtilities.h>

#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkImageGradientMagnitude.h>
#include <vtkMatrix4x4.h>
#include <vtkMetaDataSet.h>
#include <vtkMetaSurfaceMesh.h>
#include <vtkMetaVolumeMesh.h>
#include <vtkPolyData.h>
#include <vtkProbeFilter.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>

#include <itkImageToVTKImageFilter.h>

// /////////////////////////////////////////////////////////////////
// meshMappingPrivate
// /////////////////////////////////////////////////////////////////

class meshMappingPrivate
{
public:
    dtkSmartPointer <medAbstractData> structure;
    dtkSmartPointer <medAbstractData> data;
    dtkSmartPointer <medAbstractData> output;

    template <class PixelType> int mapImageOnMesh()
    {
        typedef itk::Image<PixelType, 3> ImageType;

        if ( !data ||!data->data() || !structure ||!structure->data())
        {
            return medAbstractProcessLegacy::FAILURE;
        }

        //Converting the mesh
        if(!structure->identifier().contains("vtkDataMesh"))
        {
            return medAbstractProcessLegacy::MESH_TYPE;
        }

        vtkMetaDataSet *structureDataset = static_cast<vtkMetaDataSet*>(structure->data());
        vtkPolyData *structurePolydata = static_cast<vtkPolyData*>(structureDataset->GetDataSet());

        // Converting the image
        typedef itk::ImageToVTKImageFilter<ImageType> FilterType;
        typename FilterType::Pointer filter = FilterType::New();
        typename ImageType::Pointer img = static_cast<ImageType *>(data->data());
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
        matrix->MultiplyPoint (v_origin, v_origin2);
        for (int i=0; i<3; i++)
        {
            matrix->SetElement (i, 3, v_origin[i]-v_origin2[i]);
        }
        //------------------------------------------------------

        vtkImageData *vtkImage = filter->GetOutput();

        vtkImageCast *cast = vtkImageCast::New();
        cast->SetInputData(vtkImage);
        cast->SetOutputScalarTypeToFloat();

        //To get the itkImage infos back
        vtkSmartPointer<vtkTransform> t = vtkSmartPointer<vtkTransform>::New();
        t->SetMatrix(matrix);

        vtkSmartPointer<vtkTransformFilter> transformFilter = vtkSmartPointer<vtkTransformFilter>::New();
        transformFilter->SetInputConnection(cast->GetOutputPort());
        transformFilter->SetTransform(t);

        // Probe magnitude with iso-surface.
        vtkProbeFilter *probe = vtkProbeFilter::New();
        probe->SetInputData(structurePolydata);
        probe->SetSourceConnection(transformFilter->GetOutputPort());
        probe->SpatialMatchOn();
        probe->Update();
        vtkPolyData *polydata = probe->GetPolyDataOutput();

        vtkMetaSurfaceMesh *smesh = vtkMetaSurfaceMesh::New();
        smesh->SetDataSet(polydata);

        output = medAbstractDataFactory::instance()->createSmartPointer("vtkDataMesh");
        output->setData(smesh);
        medUtilities::setDerivedMetaData(output, structure, "meshMapping");

        return medAbstractProcessLegacy::SUCCESS;
    }

    int mapMeshOnMesh()
    {
        if ( !data ||!data->data() || !structure ||!structure->data())
        {
            return medAbstractProcessLegacy::FAILURE;
        }

        //Converting the meshes
        if(!structure->identifier().contains("vtkDataMesh"))
        {
            return medAbstractProcessLegacy::MESH_TYPE;
        }

        vtkMetaDataSet *structureMetaDataSet = static_cast<vtkMetaDataSet*>(structure->data());
        vtkDataSet *structureDataSet = static_cast<vtkDataSet*>(structureMetaDataSet->GetDataSet());

        vtkMetaDataSet *dataMetaDataSet = static_cast<vtkMetaDataSet*>(data->data());
        vtkDataSet *dataDataSet = static_cast<vtkDataSet*>(dataMetaDataSet->GetDataSet());

        // Probe magnitude with iso-surface.
        vtkProbeFilter *probe = vtkProbeFilter::New();
        probe->SetInputData(structureDataSet);
        probe->SetSourceData(dataDataSet);
        probe->SpatialMatchOn();
        probe->Update();

        // create the output.
        output = medAbstractDataFactory::instance()->createSmartPointer("vtkDataMesh");
        medUtilities::setDerivedMetaData(output, structure, "meshMapping");

        vtkMetaDataSet *outputDataSet = nullptr;
        if (structureMetaDataSet->GetType() == vtkMetaDataSet::VTK_META_VOLUME_MESH)
        {
            outputDataSet = vtkMetaVolumeMesh::New();
        }
        else
        {
            outputDataSet = vtkMetaSurfaceMesh::New();
        }
        outputDataSet->SetDataSet(probe->GetOutput());
        output->setData(outputDataSet);

        probe->Delete();
        outputDataSet->Delete();

        return medAbstractProcessLegacy::SUCCESS;
    }
};

// /////////////////////////////////////////////////////////////////
// meshMapping
// /////////////////////////////////////////////////////////////////

meshMapping::meshMapping() : medAbstractProcessLegacy(), d(new meshMappingPrivate)
{
    d->data = nullptr;
    d->structure = nullptr;
    d->output = nullptr;
}

meshMapping::~meshMapping()
{
    delete d;
    d = nullptr;
}

bool meshMapping::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("meshMapping", createMeshMapping);
}

QString meshMapping::description() const
{
    return "meshMapping";
}

void meshMapping::setInput(medAbstractData *data, int channel)
{
    if (!data)
    {
        return;
    }

    if (channel == 0)
    {
        d->structure = data;
    }

    if (channel == 1)
    {
        d->data = data;
    }
}

void meshMapping::setParameter(double  data, int channel)
{
    // Here comes a switch over channel to handle parameters
}

int meshMapping::update()
{
    int res = medAbstractProcessLegacy::FAILURE;

    if (d->data)
    {
        QString id = d->data->identifier();

        if ( id == "itkDataImageChar3" )
        {
            res = d->mapImageOnMesh<char>();
        }
        else if ( id == "itkDataImageUChar3" )
        {
            res = d->mapImageOnMesh<unsigned char>();
        }
        else if ( id == "itkDataImageShort3" )
        {
            res = d->mapImageOnMesh<short>();
        }
        else if ( id == "itkDataImageUShort3" )
        {
            res = d->mapImageOnMesh<unsigned short>();
        }
        else if ( id == "itkDataImageInt3" )
        {
            res = d->mapImageOnMesh<int>();
        }
        else if ( id == "itkDataImageUInt3" )
        {
            res = d->mapImageOnMesh<unsigned int>();
        }
        else if ( id == "itkDataImageLong3" )
        {
            res = d->mapImageOnMesh<long>();
        }
        else if ( id== "itkDataImageULong3" )
        {
            res = d->mapImageOnMesh<unsigned long>();
        }
        else if ( id == "itkDataImageFloat3" )
        {
            res = d->mapImageOnMesh<float>();
        }
        else if ( id == "itkDataImageDouble3" )
        {
            res = d->mapImageOnMesh<double>();
        }
        else if ( id == "vtkDataMesh" )
        {
            res = d->mapMeshOnMesh();
        }
        else
        {
            res = medAbstractProcessLegacy::PIXEL_TYPE;
        }
    }

    return res;
}        

medAbstractData * meshMapping::output()
{
    return d->output;
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createMeshMapping()
{
    return new meshMapping;
}
