/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medVtkMeshDataWriter.h>

#include <medAbstractDataFactory.h>
#include <medAbstractData.h>
#include <dtkLog/dtkLog.h>

#include <vtkMetaDataSet.h>


const char medVtkMeshDataWriter::ID[] = "medVtkMeshDataWriter";

medVtkMeshDataWriter::medVtkMeshDataWriter()
{
}

medVtkMeshDataWriter::~medVtkMeshDataWriter()
{
}

QStringList medVtkMeshDataWriter::handled() const
{
    return QStringList() << "medVtkMeshData";
}

QStringList medVtkMeshDataWriter::s_handled()
{
    return QStringList() << "medVtkMeshData";
}

bool medVtkMeshDataWriter::canWrite(const QString& /*path*/)
{
    if ( ! this->data())
        return false;

    return dynamic_cast<vtkMetaDataSet*>((vtkObject*)(this->data()->data()));
}

bool medVtkMeshDataWriter::write(const QString& path)
{
  if (!this->data())
    return false;

  qDebug() << "Can write with: " << this->identifier();

  medAbstractData * medData = dynamic_cast<medAbstractData*>(this->data());

  if(medData->identifier() != "medVtkMeshData")
  {
    return false;
  }

  vtkMetaDataSet * mesh = dynamic_cast< vtkMetaDataSet*>( (vtkObject*)(this->data()->data()));
  if (!mesh)
    return false;

  mesh->Write(path.toLocal8Bit().constData());

  return true;
}

QString medVtkMeshDataWriter::description() const
{
    return tr( "VTK Mesh Writer" );
}

QString medVtkMeshDataWriter::identifier() const
{
    return ID;
}

QStringList medVtkMeshDataWriter::supportedFileExtensions() const
{
    return QStringList() << ".vtk" << ".vtp";
}

bool medVtkMeshDataWriter::registered()
{
  return medAbstractDataFactory::instance()->registerDataWriterType("medVtkMeshDataWriter", medVtkMeshDataWriter::s_handled(), createmedVtkMeshDataWriter);
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createmedVtkMeshDataWriter()
{
  return new medVtkMeshDataWriter;
}


