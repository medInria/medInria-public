/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include "medUtilities.h"
#include "medUtilitiesVTK.h"

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>

#include <medAbstractProcessLegacy.h>

#include <vtkCellData.h>
#include <vtkLinearTransform.h>
#include <vtkMetaDataSet.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>

medAbstractData* medUtilitiesVTK::changeMaxNumberOfMeshTriangles(medAbstractData *mesh, int maxNumber)
{
    vtkMetaDataSet * dataset = reinterpret_cast<vtkMetaDataSet*>(mesh->data());
    vtkPolyData * polydata = dynamic_cast<vtkPolyData*>(dataset->GetDataSet());

    int initialNumber = polydata->GetNumberOfPolys();

    if (maxNumber < initialNumber) // Decimate
    {
        double decimateValue = 1.0 - (double)maxNumber/(double)initialNumber;

        dtkSmartPointer<medAbstractProcessLegacy> process = dtkAbstractProcessFactory::instance()->createSmartPointer("medDecimateMeshProcess");
        process->setInput(mesh);
        process->setParameter(decimateValue);
        process->update();

        return process->output();
    }
    return mesh;
}

vtkDataArray* medUtilitiesVTK::getArray(medAbstractData* data,
                                     QString arrayName)
{
    vtkDataArray* result = nullptr;
    if (data->identifier().contains("vtkDataMesh") ||
        data->identifier().contains("EPMap"))
    {
        vtkMetaDataSet* metaData = static_cast<vtkMetaDataSet*>(data->data());
        vtkDataSet* mesh = metaData->GetDataSet();
        // try point data first
        result = mesh->GetPointData()->GetArray(qPrintable(arrayName));
        if (!result)
        {
            // try cell data
            result = mesh->GetCellData()->GetArray(qPrintable(arrayName));
            if (!result)
            {
                // try field data
                result = mesh->GetFieldData()->GetArray(qPrintable(arrayName));
            }
        }
    }
    return result;
}

int medUtilitiesVTK::getArrayIndex(medAbstractData* data,
                                QString arrayName,
                                DataArrayType* arrayType)
{
    int arrayId = -1;
    DataArrayType type = DataArrayType::UNKNOWN;
    if (data->identifier().contains("vtkDataMesh") ||
        data->identifier().contains("EPMap"))
    {
        vtkMetaDataSet* metaData = static_cast<vtkMetaDataSet*>(data->data());
        vtkDataSet* mesh = metaData->GetDataSet();
        // try point data first
        (void)mesh->GetPointData()->GetAbstractArray(qPrintable(arrayName), arrayId);
        if (arrayId == -1)
        {
            (void)mesh->GetCellData()->GetAbstractArray(qPrintable(arrayName), arrayId);
            if (arrayId == -1)
            {
                (void)mesh->GetFieldData()->GetAbstractArray(qPrintable(arrayName), arrayId);
                if (arrayId != -1)
                {
                    type = DataArrayType::FIELD_ARRAY;
                }
            }
            else
            {
                type = DataArrayType::CELL_ARRAY;
            }
        }
        else
        {
            type = DataArrayType::POINT_ARRAY;
        }
    }

    if (arrayType)
    {
        *arrayType = type;
    }

    return arrayId;
}


QList<double> medUtilitiesVTK::peekArray(medAbstractData* data,
                                      QString arrayName,
                                      int index)
{
    QList<double> result;
    vtkDataArray* array = getArray(data, arrayName);
    if (array && (index < array->GetNumberOfTuples()))
    {
        int nbComponents = array->GetNumberOfComponents();
        double* tuple = new double[nbComponents];
        array->GetTuple(index, tuple);
        for (int i = 0; i < nbComponents; ++i)
        {
            result.push_back(tuple[i]);
        }
        delete[] tuple;
    }
    return result;
}

bool medUtilitiesVTK::arrayRange(medAbstractData* data,
                              QString arrayName,
                              double* minRange,
                              double* maxRange,
                              int component)
{
    vtkDataArray* array = getArray(data, arrayName);
    if (array && (component < array->GetNumberOfComponents()))
    {
        double* range = new double[2]();
        array->GetRange(range, component);
        *minRange = range[0];
        *maxRange = range[1];
        delete[] range;
        return true;
    }
    return false;
}

bool medUtilitiesVTK::arrayStats(medAbstractData* data,
                                 QString arrayName,
                                 double* mean,
                                 double* stdDev,
                                 int component)
{
    vtkDataArray* array = getArray(data, arrayName);
    if (array && (component < array->GetNumberOfComponents()))
    {
        vtkIdType nbTuples = array->GetNumberOfTuples();
        QList<double> samples;
        double variance = 0.0;
        for (vtkIdType i = 0; i < nbTuples; ++i)
        {
            samples.append(array->GetComponent(i, component));
        }
        medUtilities::computeMeanAndVariance(samples, mean, &variance);
        *stdDev = std::sqrt(variance);
        return true;
    }
    return false;
}

void medUtilitiesVTK::transformCoordinates(medAbstractData* data,
                                           QStringList arrayNames,
                                           vtkLinearTransform* transformFilter)
{
    foreach (QString arrayName, arrayNames)
    {
        vtkDataArray* array = getArray(data, arrayName);
        if (array)
        {
            // nbComponents must be a multiple of 3
            int nbComponents = array->GetNumberOfComponents();
            if ((nbComponents % 3) == 0)
            {
                // this array contains coordinates that must be transformed
                double* inPt = new double[nbComponents];
                double* outPt = new double[nbComponents];
                for (vtkIdType k = 0; k < array->GetNumberOfTuples(); ++k)
                {
                    array->GetTuple(k, inPt);
                    for (int l = 0; l < nbComponents / 3; ++l)
                    {
                        transformFilter->InternalTransformPoint(inPt + l * 3, outPt + l * 3);
                    }
                    array->SetTuple(k, outPt);
                }
                delete[] inPt;
                delete[] outPt;
            }
        }
    }
}
