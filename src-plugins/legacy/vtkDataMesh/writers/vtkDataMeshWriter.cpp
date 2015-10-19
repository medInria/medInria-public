/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkDataMeshWriter.h>

#include <medAbstractDataFactory.h>
#include <medAbstractData.h>
#include <dtkLog/dtkLog.h>

#include <vtkMetaDataSet.h>


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

  qDebug() << "Can write with: " << this->identifier();

  medAbstractData * medData = dynamic_cast<medAbstractData*>(this->data());

  if(medData->identifier() != "vtkDataMesh")
  {
    return false;
  }

  vtkMetaDataSet * mesh = dynamic_cast< vtkMetaDataSet*>( (vtkObject*)(this->data()->data()));
  if (!mesh)
    return false;

  mesh->Write(path.toLocal8Bit().constData());

  return true;
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


