#include "vtkDataMesh.h"

#include <dtkCore/dtkAbstractDataFactory.h>

#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkUnstructuredGrid.h"

#include <vtkPNGWriter.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkWindowToImageFilter.h>

class vtkDataMeshPrivate
{
public:
  vtkSmartPointer<vtkPointSet> mesh;
  QList<QImage>          thumbnails;
};

vtkDataMesh::vtkDataMesh(): dtkAbstractDataMesh(), d (new vtkDataMeshPrivate)
{
  d->mesh = 0;
}
vtkDataMesh::~vtkDataMesh()
{
  delete d;
  d = 0;
}

bool vtkDataMesh::registered()				
{
  return dtkAbstractDataFactory::instance()->registerDataType("vtkDataMesh", createVtkDataMesh);
}

QString vtkDataMesh::description() const
{
  return "vtkDataMesh";
}

void vtkDataMesh::setData(void *data)			
{
  vtkPointSet* mesh = vtkPointSet::SafeDownCast( (vtkObject*) data );
  
  if (!mesh)
  {
    qDebug() << "Cannot cast data to correct data type";
    return;		
  }
  
  d->mesh = mesh;
}

void *vtkDataMesh::output(void)				
{
  return d->mesh;
}

void *vtkDataMesh::data(void)
{
  return d->mesh;
}

void vtkDataMesh::update(void)				
{

}

dtkAbstractData *createVtkDataMesh(void)
{
  return new vtkDataMesh;
}

QImage & vtkDataMesh::thumbnail (void) const
{
  if (!d->thumbnails.size())
    return dtkAbstractDataMesh::thumbnail();
  
  return (d->thumbnails[0]);
}

QList<QImage> & vtkDataMesh::thumbnails (void) const
{
  d->thumbnails.clear();

  if (!d->mesh->GetNumberOfPoints())
    return d->thumbnails;
  
  vtkDataSetSurfaceFilter* geometryextractor = vtkDataSetSurfaceFilter::New();
  vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
  vtkActor* actor = vtkActor::New();
  vtkProperty* prop = vtkProperty::New();
  vtkRenderer* renderer = vtkRenderer::New();
  vtkRenderWindow* window = vtkRenderWindow::New();
  vtkWindowToImageFilter* filter = vtkWindowToImageFilter::New();
  geometryextractor->SetInput (d->mesh);
  mapper->SetInput (geometryextractor->GetOutput());
  actor->SetMapper (mapper);
  actor->SetProperty (prop);  
  renderer->AddViewProp(actor);
  window->SetSize (128,128);
  window->AddRenderer (renderer);
  window->OffScreenRenderingOn();
  renderer->ResetCamera();
  window->Render();
  unsigned int w=128, h=128;

  QImage img(w, h, QImage::Format_RGB32);
  
  vtkUnsignedCharArray* pixels = vtkUnsignedCharArray::New();
  pixels->SetArray(img.bits(), w*h*4, 1);
  window->GetRGBACharPixelData(0, 0, w-1, h-1, 1, pixels);
  pixels->Delete();

  mapper->Delete();
  geometryextractor->Delete();
  actor->Delete();
  renderer->Delete();
  window->Delete();

  d->thumbnails.push_back (img);
  
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

int vtkDataMesh::countVertices(void)
{
  return 0;
}

int vtkDataMesh::countEdges(void)
{
  return 0;
}
