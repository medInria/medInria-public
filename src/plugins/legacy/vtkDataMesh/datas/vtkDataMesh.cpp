/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkDataMesh.h>
#include <vtkMetaDataSet.h>

#include <medAbstractDataFactory.h>

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkUnstructuredGrid.h>

#include <vtkPNGWriter.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkWindowToImageFilter.h>

#include <QApplication>

class vtkDataMeshPrivate
{
public:
  vtkSmartPointer<vtkMetaDataSet> mesh;
  QList<QImage>          thumbnails;
};

vtkDataMesh::vtkDataMesh(): medAbstractMeshData(), d (new vtkDataMeshPrivate)
{
  this->moveToThread(QApplication::instance()->thread());
  d->mesh = 0;
}
vtkDataMesh::~vtkDataMesh()
{
  delete d;
  d = 0;
}

bool vtkDataMesh::registered()
{
  return medAbstractDataFactory::instance()->registerDataType<vtkDataMesh>();
}

void vtkDataMesh::setData(void *data)
{
  vtkMetaDataSet * mesh = vtkMetaDataSet::SafeDownCast( (vtkObject*) data );
  if (!mesh)
  {
    dtkDebug() << "Cannot cast data to correct data type";
    return;
  }

  if ( (mesh->GetType() != vtkMetaDataSet::VTK_META_SURFACE_MESH) &&
       (mesh->GetType() != vtkMetaDataSet::VTK_META_VOLUME_MESH) )
  {
    dtkDebug() << "Cannot cast data to correct data type";
    return;
  }

  d->mesh = mesh;
}

void *vtkDataMesh::output()
{
  return d->mesh;
}

void *vtkDataMesh::data()
{
  return d->mesh;
}

void vtkDataMesh::update()
{

}

void vtkDataMesh::onMetaDataSet(const QString& key, const QString& value)
{
  Q_UNUSED(key);
  Q_UNUSED(value);
}
void vtkDataMesh::onPropertySet(const QString& key, const QString& value)
{
  Q_UNUSED(key);
  Q_UNUSED(value);
}

int vtkDataMesh::countVertices()
{
  return 0;
}

int vtkDataMesh::countEdges()
{
    return 0;
}
