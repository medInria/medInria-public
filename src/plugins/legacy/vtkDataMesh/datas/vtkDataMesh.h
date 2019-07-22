/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "vtkDataMeshPluginExport.h"

#include <medAbstractMeshData.h>
#include <dtkCoreSupport/dtkGlobal.h>

class vtkDataMeshPrivate;

class VTKDATAMESHPLUGIN_EXPORT vtkDataMesh : public medAbstractMeshData
{
    Q_OBJECT
    MED_DATA_INTERFACE("VTK Mesh Data",
                       "Mesh data based of vtkPolyData for surface meshes and vtkUnstructuredGrid for volume meshes.")
 public:
    vtkDataMesh();
    vtkDataMesh(const vtkDataMesh& other);
    ~vtkDataMesh();

    vtkDataMesh* clone() override;

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


 public:
    // derived from medAbstractImageData

 private:

    vtkDataMeshPrivate* d;
};
