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

#include <medVtkMeshExport.h>

class medVtkMeshDataPrivate;

class MEDVTKMESHPLUGIN_EXPORT medVtkMeshData : public medAbstractMeshData
{
    Q_OBJECT
    MED_DATA_INTERFACE("VTK Mesh Data",
                       "Mesh data based of vtkPolyData for surface meshes and vtkUnstructuredGrid for volume meshes.")
 public:
    medVtkMeshData();
    ~medVtkMeshData();

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

    medVtkMeshDataPrivate* d;
};
