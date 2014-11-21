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

class medVtk4DMeshDataPrivate;

class MEDVTKMESHPLUGIN_EXPORT medVtk4DMeshData : public medAbstractMeshData
{
    Q_OBJECT
    MED_DATA_INTERFACE("VTK Mesh Data 4D",
                       "Mesh time sequence.")
 public:
    medVtk4DMeshData();
    ~medVtk4DMeshData();

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
    medVtk4DMeshDataPrivate* d;

};
