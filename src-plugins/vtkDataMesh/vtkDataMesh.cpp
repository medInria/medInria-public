/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkDataMesh.h"

#include <dtkCore/dtkAbstractDataFactory.h>

#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkUnstructuredGrid.h"
#include "vtkMetaDataSetSequence.h"

#include <vtkPNGWriter.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkWindowToImageFilter.h>

#include <QVTKWidget.h>

class vtkDataMeshPrivate
{
public:
  vtkSmartPointer<vtkMetaDataSet> mesh;
  QList<QImage>          thumbnails;
};

const char vtkDataMesh::ID[] = "vtkDataMesh";

vtkDataMesh::vtkDataMesh(): medAbstractDataMesh(), d (new vtkDataMeshPrivate)
{
  //this->moveToThread(QApplication::instance()->thread());
  d->mesh = 0;
}
vtkDataMesh::~vtkDataMesh()
{
  delete d;
  d = 0;
}

bool vtkDataMesh::registered()
{
  return dtkAbstractDataFactory::instance()->registerDataType(ID,createVtkDataMesh);
}

QString vtkDataMesh::description() const
{
    return tr("vtk mesh data");
}
QString vtkDataMesh::identifier() const {
    return ID;
}

void vtkDataMesh::setData(void *data)
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

dtkAbstractData *createVtkDataMesh()
{
  return new vtkDataMesh;
}

QImage & vtkDataMesh::thumbnail()
{
    if (d->thumbnails.empty())
    {
        QList<QImage> & thumbnails = this->thumbnails();
        if (thumbnails.empty())
            return medAbstractDataMesh::thumbnail();
    }

  return (d->thumbnails[0]);
}

QList<QImage> & vtkDataMesh::thumbnails()
{
    d->thumbnails.clear();

    vtkPointSet * mesh = vtkPointSet::SafeDownCast(d->mesh->GetDataSet());

    if (!mesh || !mesh->GetNumberOfPoints())
        return d->thumbnails;

    //if ( QThread::currentThread() != QApplication::instance()->thread())
    //    QMetaObject::invokeMethod(this, "createThumbnails", Qt::BlockingQueuedConnection);
    //else
        createThumbnails();

    return d->thumbnails;
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

void vtkDataMesh::createThumbnails()
{
    vtkPointSet * mesh = vtkPointSet::SafeDownCast(d->mesh->GetDataSet());

    unsigned int w=128, h=128;
    QImage img(w, h, QImage::Format_RGB32);
    img.fill(0);

    vtkSmartPointer <vtkDataSetSurfaceFilter> geometryextractor = vtkDataSetSurfaceFilter::New();
    vtkSmartPointer <vtkPolyDataMapper> mapper = vtkPolyDataMapper::New();
    vtkSmartPointer <vtkActor> actor = vtkActor::New();
    vtkSmartPointer <vtkProperty> prop = vtkProperty::New();
    vtkSmartPointer <vtkRenderer> renderer = vtkRenderer::New();
    vtkSmartPointer <vtkRenderWindow> window = vtkRenderWindow::New();

    //ugly trick of doom to have no popups
    QVTKWidget widget;
    widget.resize(w,h);

    geometryextractor->SetInput (mesh);
    mapper->SetInput (geometryextractor->GetOutput());
    actor->SetMapper (mapper);
    actor->SetProperty (prop);
    renderer->AddViewProp(actor);
    window->SetSize (w,h);
    window->SetOffScreenRendering(1);
    window->AddRenderer (renderer);

    renderer->ResetCamera();

    widget.SetRenderWindow(window);
    window->Render();
    
    vtkSmartPointer <vtkUnsignedCharArray> pixels = vtkUnsignedCharArray::New();
    pixels->SetArray(img.bits(), w*h*4, 1);
    window->GetRGBACharPixelData(0, 0, w-1, h-1, 1, pixels);

    d->thumbnails.push_back (img);
}
