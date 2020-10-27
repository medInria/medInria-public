/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkDataMeshReaderBase.h"
#include "vtkDataMeshWriterBase.h"

#include <medMetaDataKeys.h>

#include <dtkCoreSupport/dtkAbstractData>

#include <vtkFieldData.h>
#include <vtkMetaDataSet.h>
#include <vtkStringArray.h>
#include <vtkSmartPointer.h>

vtkDataMeshReaderBase::vtkDataMeshReaderBase() : dtkAbstractDataReader()
{
    m_oldKeyToNewKey["StudyDicomID"] = medMetaDataKeys::StudyInstanceUID.key();
    m_oldKeyToNewKey["SeriesDicomID"] = medMetaDataKeys::SeriesInstanceUID.key();
}

bool vtkDataMeshReaderBase::canRead(const QStringList& paths)
{
    bool result = false;

    if (!paths.empty())
    {
        result = canRead(paths.first().toLocal8Bit().constData());
    }

    return result;
}

bool vtkDataMeshReaderBase::readInformation(const QStringList& paths)
{
    bool result = false;

    if (!paths.empty())
    {
        result = readInformation(paths.first().toLocal8Bit().constData());
    }

    return result;
}

bool vtkDataMeshReaderBase::read(const QStringList& paths)
{
    bool result = false;

    if (!paths.empty())
    {
        result = read(paths.first().toLocal8Bit().constData());
    }

    return result;
}

QString vtkDataMeshReaderBase::medMetaDataRetrocompatility(QString key)
{
    if (m_oldKeyToNewKey.contains(key))
    {
        return m_oldKeyToNewKey[key];
    }
    else
    {
        return key;
    }
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
            metaDataKey = medMetaDataRetrocompatility(metaDataKey);

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
