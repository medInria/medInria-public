/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkDataMeshReader.h>

#include <vtkErrorCode.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkSmartPointer.h>

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
    dtkSmartPointer<dtkAbstractData> dtkdata = dtkAbstractDataFactory::instance()->createSmartPointer("vtkDataMesh");
    this->setData(dtkdata);
    dtkdata->addMetaData("FilePath", QStringList() << path); // useful ?
    
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

    if (dtkAbstractData * dtkdata = data())
    {
        if (!(dtkdata->identifier() == "vtkDataMesh"))
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

        dtkdata->setData(dataSet);
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
    return dtkAbstractDataFactory::instance()->registerDataReaderType(ID,
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

