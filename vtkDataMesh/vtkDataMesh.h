#ifndef _vtk_DataMesh_h_
#define _vtk_DataMesh_h_

#include <dtkCore/dtkAbstractDataMesh.h>
#include <dtkCore/dtkGlobal.h>

#include "vtkDataMeshPluginExport.h"

class vtkDataMeshPrivate;



/**
    
    \class vtkDataMesh
    \brief This class describes the data type that handles
    a mesh in the vtk format.

    It integrates the vtk pipeline in order to create thumbnails

    The class keeps a smart pointer over the vtkPointSet instance
    which is the base class for surface meshes (vtkPolyData) and
    volumetric meshes (vtkUnstructuredGrid)
    
    \todo There is a problem in the thumbnail creation as it uses
    a vtkRenderer, therefore opens up a vtkWindow to create thumbnail,
    which should not happen.
    
    \see vtkDataMesh4D vtkDataMeshReader itkDataImage vtkPointSet
*/

class VTKDATAMESHPLUGIN_EXPORT vtkDataMesh : public dtkAbstractDataMesh
{
  Q_OBJECT
    
 public:
    vtkDataMesh(void);
    ~vtkDataMesh(void);
    virtual QString description(void) const;
    static bool registered(void);

    virtual QImage        &thumbnail  (void) const;
    virtual QList<QImage> &thumbnails (void) const;

 public slots:
    // derived from dtkAbstractData
    
    void *output(void);
    void *data(void);
    void setData(void* data);
    void update(void);

    void onMetaDataSet(const QString& key, const QString& value);
    void onPropertySet(const QString& key, const QString& value);

    int countVertices(void);
    int countEdges(void);
    
 public:
    // derived from dtkAbstractDataImage
    
 private:

    vtkDataMeshPrivate* d;
  
};

dtkAbstractData* createVtkDataMesh (void);

#endif
