/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include "meshMapping.h"

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>

#include <medAbstractDataFactory.h>
#include <medUtilities.h>

#include <vtkDoubleArray.h>
#include <vtkMetaDataSet.h>
#include <vtkMetaSurfaceMesh.h>
#include <vtkMetaVolumeMesh.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkProbeFilter.h>

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
        if ( !data ||!data->data() || !structure ||!structure->data())
        {
            return medAbstractProcessLegacy::FAILURE;
        }

        // Get the mesh polydata and create an output polydata
        if(!structure->identifier().contains("vtkDataMesh"))
        {
            return medAbstractProcessLegacy::MESH_TYPE;
        }
        vtkMetaDataSet *structureDataset = static_cast<vtkMetaDataSet*>(structure->data());
        vtkPolyData *structurePolydata = static_cast<vtkPolyData*>(structureDataset->GetDataSet());
        vtkSmartPointer<vtkPolyData> structurePolydataCopy = vtkSmartPointer<vtkPolyData>::New();
        structurePolydataCopy->DeepCopy(structurePolydata);

        // Get the ITK data
        using ImageType = itk::Image<PixelType, 3>;
        typename ImageType::Pointer img = static_cast<ImageType *>(data->data());

        // Create a new point data array for the interpolated values
        vtkSmartPointer<vtkDoubleArray> interpolatedValues = vtkSmartPointer<vtkDoubleArray>::New();
        interpolatedValues->SetName("MappedValues");
        interpolatedValues->SetNumberOfComponents(1);
        interpolatedValues->SetNumberOfTuples(structurePolydataCopy->GetNumberOfPoints());

        // Fill the data array of points from the VTK with values from the ITK data
        typename ImageType::IndexType itkIndex;
        typename ImageType::PointType itkPoint;
        double point[3];
        vtkPoints* points = structurePolydataCopy->GetPoints();
        for (vtkIdType i = 0; i < structurePolydataCopy->GetNumberOfPoints(); ++i)
        {    
            points->GetPoint(i, point);
            for (int d = 0; d < 3; ++d)
            {
                itkPoint[d] = point[d];
            }
            
            img->TransformPhysicalPointToIndex(itkPoint, itkIndex);
            typename ImageType::PixelType pixelValue = img->GetPixel(itkIndex);
            if (!std::isnan(pixelValue))
            {
                interpolatedValues->SetValue(i, static_cast<double>(pixelValue));
            }
        }
        structurePolydataCopy->GetPointData()->SetScalars(interpolatedValues);

        // Create output
        vtkMetaSurfaceMesh *smesh = vtkMetaSurfaceMesh::New();
        smesh->SetDataSet(structurePolydataCopy);

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
