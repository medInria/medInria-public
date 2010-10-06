#include "vtkDataMesh.h"

#include <dtkCore/dtkAbstractDataFactory.h>

#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkUnstructuredGrid.h"

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
  vtkPointSet* mesh = dynamic_cast<vtkPointSet*>( (vtkObject*) data );
  
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
  if (!d->mesh)
    return dtkAbstractDataMesh::thumbnail();
}

QList<QImage> & vtkDataMesh::thumbnails (void) const
{
  ///\todo generate several thumbnails !...
  return d->thumbnails;
}

void vtkDataMesh::onMetaDataSet(QString key, QString value)
{  
  Q_UNUSED(key);
  Q_UNUSED(value);
}
void vtkDataMesh::onPropertySet(QString key, QString value)
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
