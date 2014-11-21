/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medVtkMeshData.h>

#include <medAbstractDataFactory.h>

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkUnstructuredGrid.h>
#include <vtkMetaDataSetSequence.h>

#include <vtkPNGWriter.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkWindowToImageFilter.h>

#include <QVTKWidget.h>

class medVtkMeshDataPrivate
{
public:
  vtkSmartPointer<vtkMetaDataSet> mesh;
  QList<QImage>          thumbnails;
};

medVtkMeshData::medVtkMeshData(): medAbstractMeshData(), d (new medVtkMeshDataPrivate)
{
  this->moveToThread(QApplication::instance()->thread());
  d->mesh = 0;
}
medVtkMeshData::~medVtkMeshData()
{
  delete d;
  d = 0;
}

bool medVtkMeshData::registered()
{
  return medAbstractDataFactory::instance()->registerDataType<medVtkMeshData>();
}

void medVtkMeshData::setData(void *data)
{
  vtkMetaDataSet * mesh = vtkMetaDataSet::SafeDownCast( (vtkObject*) data );
  if (!mesh)
  {
    qDebug() << "Cannot cast data to correct data type";
    return;
  }

  if ( (mesh->GetType() != vtkMetaDataSet::VTK_META_SURFACE_MESH) &&
       (mesh->GetType() != vtkMetaDataSet::VTK_META_VOLUME_MESH) )
  {
    qDebug() << "Cannot cast data to correct data type";
    return;
  }

  d->mesh = mesh;
}

void *medVtkMeshData::output()
{
  return d->mesh;
}

void *medVtkMeshData::data()
{
  return d->mesh;
}

void medVtkMeshData::update()
{

}

void medVtkMeshData::onMetaDataSet(const QString& key, const QString& value)
{
  Q_UNUSED(key);
  Q_UNUSED(value);
}
void medVtkMeshData::onPropertySet(const QString& key, const QString& value)
{
  Q_UNUSED(key);
  Q_UNUSED(value);
}

int medVtkMeshData::countVertices()
{
  return 0;
}

int medVtkMeshData::countEdges()
{
    return 0;
}
