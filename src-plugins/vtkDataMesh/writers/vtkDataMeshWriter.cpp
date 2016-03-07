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

  // Get the extension of the filename
  QFileInfo pathfile(path);
  QString extension = pathfile.completeSuffix();

  if (extension == "vtk") // VTK files
  {
      QString header = getHeaderVtk();

      vtkSmartPointer<vtkPolyDataWriter> writer = vtkPolyDataWriter::New();
      writer->SetFileName(path.toUtf8().constData());
      try
      {
          writer->SetInput(mesh->GetDataSet());
          writer->SetHeader(header.toAscii().constData());
          writer->Write();
      }
      catch (vtkErrorCode::ErrorIds error)
      {
          qDebug() << "vtkDataMeshWriter: " << vtkErrorCode::GetStringFromErrorCode(error);
          return false;
      }
  }
  else if (extension == "vtp") // VTP files
  {
      addHeaderVtpToMesh(mesh);

      vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
        writer->SetFileName(path.toUtf8().constData());
      #if VTK_MAJOR_VERSION <= 5
        writer->SetInput(mesh->GetDataSet());
      #else
        writer->SetInputData(mesh->GetDataSet());
      #endif
        writer->Write();
  }
  else if ((extension == "mesh") || (extension == "obj")) // MESH or OBJ files
  {
      try
      {
          mesh->Write(path.toLocal8Bit().constData());
      } catch (...)
      {
          qDebug() << "vtkDataMeshWriter::write -> writing the vtkDataMesh failed.";
          return false;
      }
  }
  else
  {
      return false;
  }

  return true;
}

QStringList vtkDataMeshWriter::metaDataKeysToCopy()
{
    QStringList keys;

    keys << medMetaDataKeys::PatientID.key()
         << medMetaDataKeys::PatientName.key()
         << medMetaDataKeys::Age.key()
         << medMetaDataKeys::BirthDate.key()
         << medMetaDataKeys::Gender.key()
         << medMetaDataKeys::Description.key()
         << medMetaDataKeys::StudyID.key()
         << medMetaDataKeys::StudyDicomID.key()
         << medMetaDataKeys::StudyDescription.key()
         << medMetaDataKeys::Institution.key()
         << medMetaDataKeys::Referee.key()
         << medMetaDataKeys::StudyDate.key()
         << medMetaDataKeys::StudyTime.key()
         << medMetaDataKeys::Performer.key()
         << medMetaDataKeys::Report.key()
         << medMetaDataKeys::Protocol.key()
         << medMetaDataKeys::Origin.key()
         << medMetaDataKeys::AcquisitionDate.key()
         << medMetaDataKeys::AcquisitionTime.key()
         << medMetaDataKeys::Modality.key()
         << medMetaDataKeys::Orientation.key();

    return keys;
}

QString vtkDataMeshWriter::getHeaderVtk()
{
    // Create a header line with metadata
    QStringList keyList = metaDataKeysToCopy();

    QString header = QString("");
    foreach(QString key, keyList)
    {
        if (data()->metadata(key).toStdString().compare("") != 0)
        {
            header += key + QString("\t") + data()->metadata(key) + QString("\t");
        }
    }
    return header;
}

void vtkDataMeshWriter::addHeaderVtpToMesh(vtkMetaDataSet* mesh)
{
    // Add FieldData tag to the xml file
    QStringList keyList = metaDataKeysToCopy();

    foreach(QString key, keyList)
    {
        vtkSmartPointer<vtkStringArray> appendData = vtkSmartPointer<vtkStringArray>::New();
        appendData->SetName(key.toStdString().c_str());
        appendData->InsertNextValue(data()->metadata(key).toStdString().c_str());

        mesh->GetDataSet()->GetFieldData()->AddArray(appendData);
    }
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


