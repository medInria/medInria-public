/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
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
    bool result = false;

    if (this->data())
    {
        result = dynamic_cast<vtkMetaDataSet*>((vtkObject*)(this->data()->data()));
    }

    return result;
}

void vtkDataMeshWriterBase::addMetaDataAsFieldData(vtkMetaDataSet* dataSet)
{
    for(QString key : data()->metaDataList())
    {
        vtkSmartPointer<vtkStringArray> metaDataArray = vtkSmartPointer<vtkStringArray>::New();
        QString arrayName = QString(metaDataFieldPrefix) + key;
        metaDataArray->SetName(arrayName.toStdString().c_str());

        for(QString value : data()->metaDataValues(key))
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
