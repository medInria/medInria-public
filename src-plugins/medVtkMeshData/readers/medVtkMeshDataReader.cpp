/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medVtkMeshDataReader.h>

#include <vtkErrorCode.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <vtkMetaVolumeMesh.h>
#include <vtkMetaSurfaceMesh.h>

const char medVtkMeshDataReader::ID[] = "medVtkMeshDataReader";

medVtkMeshDataReader::medVtkMeshDataReader() : dtkAbstractDataReader() {
}

medVtkMeshDataReader::~medVtkMeshDataReader() {
}

QStringList medVtkMeshDataReader::handled() const {
    return QStringList() << "medVtkMeshData";
}

QStringList medVtkMeshDataReader::s_handled() {
    return QStringList() << "medVtkMeshData";
}

bool medVtkMeshDataReader::canRead(const QString& path) {
    return (vtkMetaVolumeMesh::CanReadFile(path.toLocal8Bit().constData()) != 0) ||
           (vtkMetaSurfaceMesh::CanReadFile(path.toLocal8Bit().constData()) != 0);
}

bool medVtkMeshDataReader::canRead(const QStringList& paths){
    if (paths.empty())
        return false;
    return canRead(paths.first().toLocal8Bit().constData());
}

bool medVtkMeshDataReader::readInformation(const QString& path) {
    medAbstractData *medData = medAbstractDataFactory::instance()->create("medVtkMeshData");
    this->setData(medData);
    medData->addMetaData("FilePath", QStringList() << path); // useful ?
    
    return true;
}

bool medVtkMeshDataReader::readInformation(const QStringList& paths) {
    if (paths.empty())
        return false;
    return readInformation(paths.first().toLocal8Bit().constData());
}

bool medVtkMeshDataReader::read(const QString& path) {
    setProgress(0);
    readInformation(path);
    setProgress(50);

    qDebug() << "Can read with: " << identifier();

    if (medAbstractData * medData = dynamic_cast<medAbstractData*>(data()))
    {
        if (!(medData->identifier() == "medVtkMeshData"))
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
            qDebug() << "Loading the medVtkMeshData failed, it's neither a surface or volume mesh !";
            return false;
        }

        try
        {
            dataSet->Read(path.toLocal8Bit().constData());
        } catch (...)
        {
            qDebug() << "Loading the medVtkMeshData failed, error while parsing !";
            return false;
        }

        medData->setData(dataSet);
    }

    setProgress(100);
    return true;
}

bool medVtkMeshDataReader::read(const QStringList& paths) {
    if (paths.empty())
        return false;
    return read(paths.first().toLocal8Bit().constData());
}

void medVtkMeshDataReader::setProgress(int value) {
    emit progressed(value);
}

bool medVtkMeshDataReader::registered() {
    return medAbstractDataFactory::instance()->registerDataReaderType(ID,
                                    medVtkMeshDataReader::s_handled(),
                                    createmedVtkMeshDataReader);
}

QString medVtkMeshDataReader::identifier() const {
    return ID;
}

QString medVtkMeshDataReader::description() const {
    return "Reader for vtk meshes";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createmedVtkMeshDataReader() {
    return new medVtkMeshDataReader;
}

