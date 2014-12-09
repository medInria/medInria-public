/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medVtk4DMeshData.h>

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

class medVtk4DMeshDataPrivate
{
public:
  vtkSmartPointer<vtkMetaDataSetSequence> meshsequence;
};

medVtk4DMeshData::medVtk4DMeshData(): medAbstractMeshData(), d (new medVtk4DMeshDataPrivate)
{
  this->moveToThread(QApplication::instance()->thread());
  d->meshsequence = NULL;
}
medVtk4DMeshData::~medVtk4DMeshData()
{
  delete d;
  d = NULL;
}

bool medVtk4DMeshData::registered()
{
  return medAbstractDataFactory::instance()->registerDataType<medVtk4DMeshData>();
}

void medVtk4DMeshData::setData(void *data)
{
  vtkMetaDataSetSequence* sequence = vtkMetaDataSetSequence::SafeDownCast( (vtkObject*) data );
  if (!sequence)
  {
    qDebug() << "Cannot cast data to correct data type";
    return;
  }

  if ( (sequence->GetType() != vtkMetaDataSet::VTK_META_SURFACE_MESH) &&
       (sequence->GetType() != vtkMetaDataSet::VTK_META_VOLUME_MESH) )
  {
    qDebug() << "Cannot cast data to correct data type";
    return;
  }

  d->meshsequence = sequence;
}

void *medVtk4DMeshData::output()
{
  return d->meshsequence;
}

void *medVtk4DMeshData::data()
{
  return d->meshsequence;
}

void medVtk4DMeshData::update()
{

}

void medVtk4DMeshData::onMetaDataSet(const QString& key, const QString& value)
{
  Q_UNUSED(key);
  Q_UNUSED(value);
}
void medVtk4DMeshData::onPropertySet(const QString& key, const QString& value)
{
  Q_UNUSED(key);
  Q_UNUSED(value);
}

int medVtk4DMeshData::countVertices()
{
  return 0;
}

int medVtk4DMeshData::countEdges()
{
    return 0;
}
