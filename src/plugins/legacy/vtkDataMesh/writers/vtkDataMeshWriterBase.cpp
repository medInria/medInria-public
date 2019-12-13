#include "vtkDataMeshWriterBase.h"

#include <dtkCoreSupport/dtkAbstractData>

#include <vtkFieldData.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>

const QString vtkDataMeshWriterBase::metaDataFieldPrefix = "medMetaData::";

vtkDataMeshWriterBase::vtkDataMeshWriterBase() : dtkAbstractDataWriter()
{
}

bool vtkDataMeshWriterBase::canWrite(const QString& /*path*/)
{
    if (!this->data())
    {
        return false;
    }
    return dynamic_cast<vtkMetaDataSet*>((vtkObject*)(this->data()->data()));
}

void vtkDataMeshWriterBase::addMetaDataAsFieldData(vtkMetaDataSet* dataSet)
{
    foreach (QString key, data()->metaDataList())
    {
        vtkSmartPointer<vtkStringArray> metaDataArray = vtkSmartPointer<vtkStringArray>::New();
        QString arrayName = QString(metaDataFieldPrefix) + key;
        metaDataArray->SetName(arrayName.toStdString().c_str());

        foreach (QString value, data()->metaDataValues(key))
        {
            metaDataArray->InsertNextValue(value.toStdString().c_str());
        }

        dataSet->GetDataSet()->GetFieldData()->AddArray(metaDataArray);
    }
}

void vtkDataMeshWriterBase::clearMetaDataFieldData(vtkMetaDataSet* dataSet)
{
    vtkFieldData* fieldData = dataSet->GetDataSet()->GetFieldData();
    vtkSmartPointer<vtkFieldData> newFieldData = vtkSmartPointer<vtkFieldData>::New();

    for (int i = 0; i < fieldData->GetNumberOfArrays(); i++)
    {
        QString arrayName = fieldData->GetArrayName(i);

        if (!arrayName.startsWith(metaDataFieldPrefix))
        {
            newFieldData->AddArray(fieldData->GetAbstractArray(i));
        }
    }

    dataSet->GetDataSet()->SetFieldData(newFieldData);
}
