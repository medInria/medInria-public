/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkDataMeshReader.h"

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>
#include <medMetaDataKeys.h>

#include <vtkErrorCode.h>
#include <vtkMetaVolumeMesh.h>
#include <vtkMetaSurfaceMesh.h>
#include <vtkPolyDataReader.h>
#include <vtkSmartPointer.h>

const char vtkDataMeshReader::ID[] = "vtkDataMeshReader";

vtkDataMeshReader::vtkDataMeshReader() : vtkDataMeshReaderBase()
{
}

QStringList vtkDataMeshReader::handled() const
{
    return QStringList() << "vtkDataMesh";
}

QStringList vtkDataMeshReader::s_handled()
{
    return QStringList() << "vtkDataMesh";
}

bool vtkDataMeshReader::canRead(const QString& path)
{
    return (vtkMetaVolumeMesh::CanReadFile(path.toLocal8Bit().constData()) != 0) ||
           (vtkMetaSurfaceMesh::CanReadFile(path.toLocal8Bit().constData()) != 0);
}

bool vtkDataMeshReader::readInformation(const QString& path)
{
    medAbstractData *medData = medAbstractDataFactory::instance()->create("vtkDataMesh");
    this->setData(medData);
    
    return true;
}

bool vtkDataMeshReader::read(const QString& path)
{
    setProgress(0);
    readInformation(path);
    setProgress(50);

    qDebug().noquote() << "Can read with: " << this->identifier();

    if (medAbstractData * medData = dynamic_cast<medAbstractData*>(data()))
    {
        if (!(medData->identifier() == "vtkDataMesh"))
        {
            return false;
        }

        vtkSmartPointer<vtkMetaDataSet> dataSet = nullptr;
        if (vtkMetaVolumeMesh::CanReadFile(path.toLocal8Bit().constData()) != 0)
        {
            dataSet = vtkSmartPointer<vtkMetaVolumeMesh>::New();
        }
        else if ( vtkMetaSurfaceMesh::CanReadFile(path.toLocal8Bit().constData()) != 0)
        {
            dataSet = vtkSmartPointer<vtkMetaSurfaceMesh>::New();
        }
        else
        {
            qDebug() << metaObject()->className() << ": loading " << path << " failed, not a surface or volume mesh";
            return false;
        }

        try
        {
            dataSet->Read(path.toLocal8Bit().constData());
            medData->setData(dataSet);

            if (!extractMetaData(path, dataSet))
            {
                qDebug() << metaObject()->className() << ": no metadata found in " << path;
            }
        }
        catch (vtkErrorCode::ErrorIds error)
        {
            qDebug() << metaObject()->className() << ": " << vtkErrorCode::GetStringFromErrorCode(error);
            return false;
        }
        catch (...)
        {
            qDebug() << metaObject()->className() << ": error reading " << path;;
            return false;
        }

        // Use filename as series description if no meta data could be found
        if (medData->metadata(medMetaDataKeys::SeriesDescription.key()).isEmpty())
        {
            QFileInfo file(path);
            medData->setMetaData(medMetaDataKeys::SeriesDescription.key(), file.baseName());
        }

        setProgress(100);
        return true;
    }
    else
    {
        return false;
    }
}

bool vtkDataMeshReader::extractMetaData(QString path, vtkMetaDataSet* dataSet)
{
    bool result = false;

    if (extractMetaDataFromFieldData(dataSet) || extractMetaDataFromHeader(path, dataSet) || extractCartoMetaData(dataSet))
    {
        result = true;
    }

    return result;
}

// A previous version of vtkDataMeshWriter stored metadata in the header
bool vtkDataMeshReader::extractMetaDataFromHeader(QString path, vtkMetaDataSet* dataSet)
{
    QString header;
    vtkSmartPointer<vtkPolyDataReader> reader = vtkPolyDataReader::New();
    reader->SetFileName(path.toLocal8Bit().constData());
    reader->Update();
    header = reader->GetHeader();
    QStringList headerElements = header.split("\t");

    for (int i = 0; i < headerElements.count() - 1; i += 2)
    {
        data()->setMetaData(headerElements.at(i), headerElements.at(i + 1));
    }

    return headerElements.count() >= 2;
}

// For CARTO files the patient name and ID is stored in the header and retrieved by vtkMetaSurfaceMesh::ReadVtkFile
bool vtkDataMeshReader::extractCartoMetaData(vtkMetaDataSet* dataSet)
{
    std::string patientName, patientID;

    bool result = false;

    if (dataSet->GetMetaData("PatientName", patientName) && dataSet->GetMetaData("PatientID", patientID))
    {
        data()->setMetaData(medMetaDataKeys::PatientName.key(), QString::fromStdString(patientName));
        data()->setMetaData(medMetaDataKeys::PatientID.key(), QString::fromStdString(patientID));

        result = true;
    }

    return result;
}

bool vtkDataMeshReader::registered()
{
    return medAbstractDataFactory::instance()->registerDataReaderType(ID,
                                    vtkDataMeshReader::s_handled(),
                                    createVtkDataMeshReader);
}

QString vtkDataMeshReader::identifier() const
{
    return ID;
}

QString vtkDataMeshReader::description() const
{
    return "Reader for vtk meshes";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createVtkDataMeshReader()
{
    return new vtkDataMeshReader;
}

