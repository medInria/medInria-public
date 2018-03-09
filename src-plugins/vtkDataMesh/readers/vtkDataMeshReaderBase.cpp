#include "vtkDataMeshReaderBase.h"
#include "vtkDataMeshWriterBase.h"

#include <dtkCore/dtkAbstractData>

#include <vtkFieldData.h>
#include <vtkMetaDataSet.h>
#include <vtkStringArray.h>
#include <vtkSmartPointer.h>

vtkDataMeshReaderBase::vtkDataMeshReaderBase() : dtkAbstractDataReader()
{
}

bool vtkDataMeshReaderBase::canRead(const QStringList& paths)
{
    if (paths.empty())
    {
        return false;
    }
    return canRead(paths.first().toLocal8Bit().constData());
}

bool vtkDataMeshReaderBase::readInformation(const QStringList& paths)
{
    if (paths.empty())
    {
        return false;
    }
    return readInformation(paths.first().toLocal8Bit().constData());
}

bool vtkDataMeshReaderBase::read(const QStringList& paths)
{
    if (paths.empty())
    {
        return false;
    }
    return read(paths.first().toLocal8Bit().constData());
}

bool vtkDataMeshReaderBase::extractMetaDataFromFieldData(vtkMetaDataSet* dataSet)
{
    bool foundMetaData = false;
    vtkFieldData* fieldData = dataSet->GetDataSet()->GetFieldData();
    vtkSmartPointer<vtkFieldData> newFieldData = vtkSmartPointer<vtkFieldData>::New();

    for (int i = 0; i < fieldData->GetNumberOfArrays(); i++)
    {
        QString arrayName = fieldData->GetArrayName(i);

        if (arrayName.startsWith(vtkDataMeshWriterBase::metaDataFieldPrefix))
        {
            foundMetaData = true;
            vtkStringArray* array = static_cast<vtkStringArray*>(fieldData->GetAbstractArray(i));
            QString metaDataKey = arrayName.remove(0, vtkDataMeshWriterBase::metaDataFieldPrefix.length());

            for (int j = 0; j < array->GetSize(); j++)
            {
                data()->addMetaData(metaDataKey, QString(array->GetValue(j)));
            }
        }
        else
        {
            newFieldData->AddArray(fieldData->GetAbstractArray(i));
        }
    }

    dataSet->GetDataSet()->SetFieldData(newFieldData);

    return foundMetaData;
}

void vtkDataMeshReaderBase::setProgress(int value)
{
    emit progressed(value);
}
