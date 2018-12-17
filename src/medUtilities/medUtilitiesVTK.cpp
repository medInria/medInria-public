#include "medUtilitiesVTK.h"

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <medAbstractData.h>
#include <medAbstractProcess.h>
#include <vtkCellData.h>
#include <vtkDataArray.h>
#include <vtkDataSet.h>
#include <vtkFieldData.h>
#include <vtkMetaDataSet.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkRenderer.h>

medAbstractData* medUtilitiesVTK::changeMaxNumberOfMeshTriangles(medAbstractData *mesh, int maxNumber)
{
    vtkMetaDataSet * dataset = reinterpret_cast<vtkMetaDataSet*>(mesh->data());
    vtkPolyData * polydata = dynamic_cast<vtkPolyData*>(dataset->GetDataSet());

    int initialNumber = polydata->GetNumberOfPolys();

    if (maxNumber < initialNumber) // Decimate
    {
        double decimateValue = 1.0 - (double)maxNumber/(double)initialNumber;

        dtkSmartPointer<medAbstractProcess> process = dtkAbstractProcessFactory::instance()->createSmartPointer("medDecimateMeshProcess");
        process->setInput(mesh,0);
        process->setParameter(decimateValue,0);
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
    DataArrayType type;
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
        // compute mean and variance
        double value, delta, finalMean, variance;
        value = delta =  finalMean = variance = 0.0;
        for (vtkIdType i = 0; i < nbTuples; ++i)
        {
            value = array->GetComponent(i, component);
            delta = value - finalMean;
            finalMean += delta / (i + 1);
            variance += delta * (value - finalMean);
        }
        if (nbTuples > 1)
        {
            variance /= nbTuples - 1;
        }
        else
        {
            variance = 0.0;
        }

        *mean = finalMean;
        *stdDev = std::sqrt(variance);
        return true;
    }
    return false;
}
