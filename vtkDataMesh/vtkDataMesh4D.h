#ifndef _vtk_DataMesh4D_h_
#define _vtk_DataMesh4D_h_

#include <dtkCore/dtkAbstractDataMesh.h>
#include <dtkCore/dtkGlobal.h>

#include "vtkDataMeshPluginExport.h"

class vtkDataMesh4DPrivate;


/**
    
    \class vtkDataMesh4D
    \brief This class describes the data type that handles
    a sequence of meshes in the vtk format.

    It integrates the vtk pipeline in order to create a single 
    thumbnail from the first iteration of the sequence
    
    The class keeps a smart pointer over the vtkMetaDataSetSequence
    instance (in vtkINRIA3D repository) that handles a list of either
    surface or volumetric meshes.
    
    \todo There is a problem in the thumbnail creation as it uses
    a vtkRenderer, therefore opens up a vtkWindow to create thumbnail,
    which should not happen.
    
    \see vtkDataMesh vtkDataMeshReader vtkMetaDataSetSequence
    \author Nicolas Toussaint
*/

class VTKDATAMESHPLUGIN_EXPORT vtkDataMesh4D : public dtkAbstractDataMesh
{
  Q_OBJECT
    
 public:
    vtkDataMesh4D(void);
    ~vtkDataMesh4D(void);
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

    vtkDataMesh4DPrivate* d;
  
};

dtkAbstractData* createVtkDataMesh4D (void);

#endif
