/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractMeshData.h>
#include <dtkCore/dtkGlobal.h>

#include <vtkDataMeshPluginExport.h>

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

class VTKDATAMESHPLUGIN_EXPORT vtkDataMesh4D : public medAbstractMeshData
{
  Q_OBJECT

 public:
    vtkDataMesh4D();
    ~vtkDataMesh4D();
    virtual QString description() const;
    virtual QString identifier() const;
    static bool registered();

 public slots:
    // derived from dtkAbstractData

    void *output();
    void *data();
    void setData(void* data);
    void update();

    void onMetaDataSet(const QString& key, const QString& value);
    void onPropertySet(const QString& key, const QString& value);

    int countVertices();
    int countEdges();


private:

    static const char ID[];
    vtkDataMesh4DPrivate* d;

};

medAbstractData* createVtkDataMesh4D();


