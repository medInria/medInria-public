/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkDataMesh4D.h>

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

class vtkDataMesh4DPrivate
{
public:
  vtkSmartPointer<vtkMetaDataSetSequence> meshsequence;
  QList<QImage>                           thumbnails;
};

const char vtkDataMesh4D::ID[] = "vtkDataMesh4D";

vtkDataMesh4D::vtkDataMesh4D(): medAbstractMeshData(), d (new vtkDataMesh4DPrivate)
{
  this->moveToThread(QApplication::instance()->thread());
  d->meshsequence = 0;
}
vtkDataMesh4D::~vtkDataMesh4D()
{
  delete d;
  d = 0;
}

bool vtkDataMesh4D::registered()				
{
  return medAbstractDataFactory::instance()->registerDataType(ID, createVtkDataMesh4D);
}

QString vtkDataMesh4D::identifier() const {
    return ID;
}

QString vtkDataMesh4D::description() const {
    return "vtkDataMesh4D";
}

void vtkDataMesh4D::setData(void *data)			
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

void *vtkDataMesh4D::output()
{
  return d->meshsequence;
}

void *vtkDataMesh4D::data()
{
  return d->meshsequence;
}

void vtkDataMesh4D::update()
{

}

medAbstractData *createVtkDataMesh4D()
{
  return new vtkDataMesh4D;
}


void vtkDataMesh4D::onMetaDataSet(const QString& key, const QString& value)
{
  Q_UNUSED(key);
  Q_UNUSED(value);
}
void vtkDataMesh4D::onPropertySet(const QString& key, const QString& value)
{
  Q_UNUSED(key);
  Q_UNUSED(value);
}

int vtkDataMesh4D::countVertices()
{
  return 0;
}

int vtkDataMesh4D::countEdges()
{
    return 0;
}
