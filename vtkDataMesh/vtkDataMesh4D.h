#ifndef _vtk_DataMesh4D_h_
#define _vtk_DataMesh4D_h_

#include <dtkCore/dtkAbstractDataMesh.h>
#include <dtkCore/dtkGlobal.h>

#include "vtkDataMesh4DPluginExport.h"

class vtkDataMesh4DPrivate;

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
