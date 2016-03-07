/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkDataMeshReader.h>

#include <medAbstractDataFactory.h>
#include <medMetaDataKeys.h>

#include <vtkErrorCode.h>
#include <vtkFieldData.h>
#include <vtkMetaVolumeMesh.h>
#include <vtkMetaSurfaceMesh.h>
#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkStringArray.h>
#include <vtkXMLPolyDataReader.h>

const char vtkDataMeshReader::ID[] = "vtkDataMeshReader";

vtkDataMeshReader::vtkDataMeshReader() : dtkAbstractDataReader() {
}

vtkDataMeshReader::~vtkDataMeshReader() {
}

QStringList vtkDataMeshReader::handled() const {
    return QStringList() << "vtkDataMesh";
}

QStringList vtkDataMeshReader::s_handled() {
    return QStringList() << "vtkDataMesh";
}

bool vtkDataMeshReader::canRead(const QString& path) {
    return (vtkMetaVolumeMesh::CanReadFile(path.toLocal8Bit().constData()) != 0) ||
           (vtkMetaSurfaceMesh::CanReadFile(path.toLocal8Bit().constData()) != 0);
}

bool vtkDataMeshReader::canRead(const QStringList& paths){
    if (paths.empty())
        return false;
    return canRead(paths.first().toLocal8Bit().constData());
}

bool vtkDataMeshReader::readInformation(const QString& path) {
    medAbstractData *medData = medAbstractDataFactory::instance()->create("vtkDataMesh");
    this->setData(medData);
    
    return true;
}

bool vtkDataMeshReader::readInformation(const QStringList& paths) {
    if (paths.empty())
        return false;
    return readInformation(paths.first().toLocal8Bit().constData());
}

bool vtkDataMeshReader::read(const QString& path) {
    setProgress(0);
    readInformation(path);
    setProgress(50);

    if (medAbstractData * medData = dynamic_cast<medAbstractData*>(data()))
    {
        if (!(medData->identifier() == "vtkDataMesh"))
            return false;

        vtkMetaDataSet * dataSet = NULL;
        if (vtkMetaVolumeMesh::CanReadFile(path.toLocal8Bit().constData()) != 0)
        {
            dataSet = vtkMetaVolumeMesh::New();
        }
        else if ( vtkMetaSurfaceMesh::CanReadFile(path.toLocal8Bit().constData()) != 0)
        {
            dataSet = vtkMetaSurfaceMesh::New();
        }
        else
        {
            qDebug() << "Loading the vtkDataMesh failed, it's neither a surface or volume mesh !";
            return false;
        }

        // Get the extension of the filename
        QFileInfo pathfile(path);
        QString extension = pathfile.completeSuffix();

        if (extension == "vtk") // VTK files
        {
            // Extract data and header from the file
            QString header;
            vtkSmartPointer<vtkPolyDataReader> reader = vtkPolyDataReader::New();
            try
            {
                reader->SetFileName(path.toLocal8Bit().constData());
                reader->Update();

                header = reader->GetHeader();
                dataSet->Read(path.toLocal8Bit().constData());
            }
            catch (vtkErrorCode::ErrorIds error)
            {
                qDebug() << "vtkDataMeshReader: " << vtkErrorCode::GetStringFromErrorCode(error);
                return false;
            }

            medData->setData(dataSet);

            // Parse header and save metadata in medData
            parseHeaderVtk(header, medData);
        }
        else if (extension == "vtp") // VTP files
        {
            dataSet->Read(path.toLocal8Bit().constData());
            medData->setData(dataSet);

            // Extract field data from the xml file
            vtkSmartPointer<vtkXMLPolyDataReader> reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
            reader->SetFileName(path.toLocal8Bit().constData());
            reader->Update();

            if (reader->GetOutput()->GetFieldData())
            {
                parseHeaderVtp(reader->GetOutput()->GetFieldData(), medData);
            }
        }
        else if ((extension == "mesh") || (extension == "obj")) // MESH or OBJ files
        {
            try
            {
                dataSet->Read(path.toLocal8Bit().constData());
            } catch (...)
            {
                qDebug() << "vtkDataMeshReader::read -> loading the vtkDataMesh failed, error while parsing !";
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    setProgress(100);
    return true;
}

QStringList vtkDataMeshReader::metaDataKeysToCopy()
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

void vtkDataMeshReader::parseHeaderVtk(QString header, medAbstractData* medData)
{
    QStringList list = header.split("\t");

    if (list.count() > 1) // Regular VTK from MUSIC
    {
        QStringList keyList = metaDataKeysToCopy();
        foreach(QString key, keyList)
        {
            if (list.contains(key))
            {
                medData->setMetaData(key, list.at(list.indexOf(key)+1));
            }
        }
    }
    else // Carto VTK
    {
        QStringList cartoList = header.split(" ");

        if (cartoList.count() == 4)
        {
            medData->setMetaData(medMetaDataKeys::PatientName.key(), cartoList.at(1)+QString("^")+cartoList.at(2));
            medData->setMetaData(medMetaDataKeys::PatientID.key(),   cartoList.at(3));
        }
    }
}

void vtkDataMeshReader::parseHeaderVtp(vtkSmartPointer<vtkFieldData> field, medAbstractData* medData)
{
    QStringList keyList = metaDataKeysToCopy();
    foreach(QString key, keyList)
    {
        if (field->HasArray(key.toStdString().c_str()))
        {
            vtkAbstractArray* subArr = field->GetAbstractArray(key.toStdString().c_str());
            medData->setMetaData(key, QString(subArr->GetVariantValue(0).ToString()));
        }
    }
}

bool vtkDataMeshReader::read(const QStringList& paths) {
    if (paths.empty())
        return false;
    return read(paths.first().toLocal8Bit().constData());
}

void vtkDataMeshReader::setProgress(int value) {
    emit progressed(value);
}

bool vtkDataMeshReader::registered() {
    return medAbstractDataFactory::instance()->registerDataReaderType(ID,
                                    vtkDataMeshReader::s_handled(),
                                    createVtkDataMeshReader);
}

QString vtkDataMeshReader::identifier() const {
    return ID;
}

QString vtkDataMeshReader::description() const {
    return "Reader for vtk meshes";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createVtkDataMeshReader() {
    return new vtkDataMeshReader;
}

