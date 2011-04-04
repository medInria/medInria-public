#include "vtkDataMesh4D.h"

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

class vtkDataMesh4DPrivate
{
public:
  vtkSmartPointer<vtkMetaDataSetSequence> meshsequence;
  QList<QImage>                           thumbnails;
};

vtkDataMesh4D::vtkDataMesh4D(): dtkAbstractDataMesh(), d (new vtkDataMesh4DPrivate)
{
  d->meshsequence = 0;
}
vtkDataMesh4D::~vtkDataMesh4D()
{
  delete d;
  d = 0;
}

bool vtkDataMesh4D::registered()				
{
  return dtkAbstractDataFactory::instance()->registerDataType("vtkDataMesh4D", createVtkDataMesh4D);
}

QString vtkDataMesh4D::description() const
{
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

void *vtkDataMesh4D::output(void)				
{
  return d->meshsequence;
}

void *vtkDataMesh4D::data(void)
{
  return d->meshsequence;
}

void vtkDataMesh4D::update(void)				
{

}

dtkAbstractData *createVtkDataMesh4D(void)
{
  return new vtkDataMesh4D;
}

QImage & vtkDataMesh4D::thumbnail (void) const
{
  if (!d->thumbnails.size())
    return dtkAbstractDataMesh::thumbnail();
  
  return (d->thumbnails[0]);
}

QList<QImage> & vtkDataMesh4D::thumbnails (void) const
{
  d->thumbnails.clear();

  vtkPointSet* mesh = vtkPointSet::SafeDownCast (d->meshsequence->GetDataSet());
  
  if (!mesh || !mesh->GetNumberOfPoints())
    return d->thumbnails;
  
  vtkDataSetSurfaceFilter* geometryextractor = vtkDataSetSurfaceFilter::New();
  vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
  vtkActor* actor = vtkActor::New();
  vtkProperty* prop = vtkProperty::New();
  vtkRenderer* renderer = vtkRenderer::New();
  vtkRenderWindow* window = vtkRenderWindow::New();
  // vtkWindowToImageFilter* filter = vtkWindowToImageFilter::New();
  geometryextractor->SetInput (mesh);
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

int vtkDataMesh4D::countVertices(void)
{
  return 0;
}

int vtkDataMesh4D::countEdges(void)
{
  return 0;
}
