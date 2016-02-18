/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkDataMeshReader.h>

#include <vtkErrorCode.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <vtkMetaVolumeMesh.h>
#include <vtkMetaSurfaceMesh.h>

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
    medData->addMetaData("FilePath", QStringList() << path); // useful ?
    
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

    qDebug() << "Can read with: " << identifier();

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

        try
        {
            dataSet->Read(path.toLocal8Bit().constData());
        } catch (...)
        {
            qDebug() << "Loading the vtkDataMesh failed, error while parsing !";
            return false;
        }

        medData->setData(dataSet);
        std::string patientName, patientID;

        if (dataSet->GetMetaData("PatientName", patientName))
            medData->setMetaData("PatientName", QString::fromStdString(patientName));
        if (dataSet->GetMetaData("PatientID", patientID))
            medData->setMetaData("PatientID", QString::fromStdString(patientID));
    }

    setProgress(100);
    return true;
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

