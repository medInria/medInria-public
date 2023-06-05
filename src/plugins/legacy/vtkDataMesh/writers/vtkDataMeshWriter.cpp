/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkDataMeshWriter.h"

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

const char vtkDataMeshWriter::ID[] = "vtkDataMeshWriter";

vtkDataMeshWriter::vtkDataMeshWriter() : vtkDataMeshWriterBase()
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

bool vtkDataMeshWriter::write(const QString& path)
{
    if (!this->data())
    {
        return false;
    }

    qDebug().noquote() << "Can write with: " << this->identifier();

    medAbstractData * medData = dynamic_cast<medAbstractData*>(this->data());

    if(medData->identifier() != "vtkDataMesh")
    {
        return false;
    }

    vtkMetaDataSet * mesh = dynamic_cast< vtkMetaDataSet*>( (vtkObject*)(this->data()->data()));
    if (!mesh)
    {
        return false;
    }
    addMetaDataAsFieldData(mesh);

    try
    {
        setlocale(LC_NUMERIC, "C");
        QLocale::setDefault(QLocale("C"));

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

QString vtkDataMeshWriter::description() const
{
    return tr( "VTK with metadata Mesh Writer" );
}

QString vtkDataMeshWriter::identifier() const
{
    return ID;
}

QStringList vtkDataMeshWriter::supportedFileExtensions() const
{
    return QStringList() << ".vtk" << ".vtp" << ".vtu";
}

bool vtkDataMeshWriter::registered()
{
  return medAbstractDataFactory::instance()->registerDataWriterType(QString("vtkDataMeshWriter"), 
                                                                    vtkDataMeshWriter::s_handled(), 
                                                                    createVtkDataMeshWriter);
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createVtkDataMeshWriter()
{
  return new vtkDataMeshWriter;
}


