/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkDataMeshWriter.h>

#include <dtkLog/dtkLog.h>

#include <medAbstractDataFactory.h>
#include <medAbstractData.h>
#include <medMetaDataKeys.h>

#include <vtkErrorCode.h>
#include <vtkFieldData.h>
#include <vtkPolyDataWriter.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkXMLPolyDataWriter.h>

const char vtkDataMeshWriter::ID[] = "vtkDataMeshWriter";

vtkDataMeshWriter::vtkDataMeshWriter()
{
}

vtkDataMeshWriter::~vtkDataMeshWriter()
{
}

QStringList vtkDataMeshWriter::handled() const
{
    return QStringList() << "vtkDataMesh";
}

QStringList vtkDataMeshWriter::s_handled()
{
    return QStringList() << "vtkDataMesh";
}

bool vtkDataMeshWriter::canWrite(const QString& /*path*/)
{
    if ( ! this->data())
        return false;

    return dynamic_cast<vtkMetaDataSet*>((vtkObject*)(this->data()->data()));
}

bool vtkDataMeshWriter::write(const QString& path)
{
  if (!this->data())
    return false;

  medAbstractData * medData = dynamic_cast<medAbstractData*>(this->data());

  if(medData->identifier() != "vtkDataMesh")
  {
    return false;
  }

  vtkMetaDataSet * mesh = dynamic_cast< vtkMetaDataSet*>( (vtkObject*)(this->data()->data()));
  if (!mesh)
    return false;

  addMetaDataAsFieldData(mesh);

  try
  {
      mesh->Write(path.toLocal8Bit().constData());
      clearMetaDataFieldData(mesh);
  }
  catch (...)
  {
      qDebug() << metaObject()->className() << ": error writing to " << path;
      clearMetaDataFieldData(mesh);
      return false;
  }

  return true;
}

void vtkDataMeshWriter::addMetaDataAsFieldData(vtkMetaDataSet* dataSet)
{
    foreach (QString key, data()->metaDataList())
    {
        vtkSmartPointer<vtkStringArray> metaDataArray = vtkSmartPointer<vtkStringArray>::New();
        QString arrayName = QString("medMetaData::") + key;
        metaDataArray->SetName(arrayName.toStdString().c_str());

        foreach (QString value, data()->metaDataValues(key))
        {
            metaDataArray->InsertNextValue(value.toStdString().c_str());
        }

        dataSet->GetDataSet()->GetFieldData()->AddArray(metaDataArray);
        metaDataFields.append(arrayName);
    }
}

void vtkDataMeshWriter::clearMetaDataFieldData(vtkMetaDataSet* dataSet)
{
    vtkFieldData* fieldData = dataSet->GetDataSet()->GetFieldData();

    foreach (QString arrayName, metaDataFields)
    {
        fieldData->RemoveArray(arrayName.toStdString().c_str());
    }

    vtkSmartPointer<vtkFieldData> newFieldData = vtkSmartPointer<vtkFieldData>::New();
    newFieldData->PassData(fieldData);
    dataSet->GetDataSet()->SetFieldData(newFieldData);
}

QString vtkDataMeshWriter::description() const
{
    return tr( "VTK Mesh Writer" );
}

QString vtkDataMeshWriter::identifier() const
{
    return ID;
}

QStringList vtkDataMeshWriter::supportedFileExtensions() const
{
    return QStringList() << ".vtk" << ".vtp";
}

bool vtkDataMeshWriter::registered()
{
  return medAbstractDataFactory::instance()->registerDataWriterType("vtkDataMeshWriter", vtkDataMeshWriter::s_handled(), createVtkDataMeshWriter);
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createVtkDataMeshWriter()
{
  return new vtkDataMeshWriter;
}


