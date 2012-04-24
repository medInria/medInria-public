#include "vtkDataMeshWriter.h"

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkLog.h>

#include <vtkMetaDataSet.h>


const char vtkDataMeshWriter::ID[] = "vtkDataMeshWriter";

vtkDataMeshWriter::vtkDataMeshWriter()
{
}

vtkDataMeshWriter::~vtkDataMeshWriter()
{
}

QStringList vtkDataMeshWriter::handled(void) const
{
    return QStringList() << "vtkDataMesh";
}

QStringList vtkDataMeshWriter::s_handled(void)
{
    return QStringList() << "vtkDataMesh";
}

bool vtkDataMeshWriter::canWrite(const QString& path)
{
  return path.endsWith (QString (".vtk"));
}

bool vtkDataMeshWriter::write(const QString& path)
{
  if (!this->data())
    return false;

  qDebug() << "Can write with: " << this->identifier();

  dtkAbstractData * dtkdata = this->data();

  if(dtkdata->identifier() != "vtkDataMesh")
  {
    return false;
  }

  vtkMetaDataSet * mesh = dynamic_cast< vtkMetaDataSet*>( (vtkObject*)(this->data()->data()));
  if (!mesh)
    return false;

  mesh->Write(path.toLocal8Bit().constData());

  return true;
}

QString vtkDataMeshWriter::description(void) const
{
    return tr( "VTK Mesh Writer" );
}

QString vtkDataMeshWriter::identifier(void) const
{
    return ID;
}

bool vtkDataMeshWriter::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataWriterType("vtkDataMeshWriter", vtkDataMeshWriter::s_handled(), createVtkDataMeshWriter);
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createVtkDataMeshWriter(void)
{
  return new vtkDataMeshWriter;
}


