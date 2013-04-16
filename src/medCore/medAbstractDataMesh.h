/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medCoreExport.h"
#include <medAbstractData.h>

class medAbstractDataVertex;
class medAbstractDataEdge;
class medAbstractDataMeshPrivate;

class MEDCORE_EXPORT medAbstractDataMesh : public medAbstractData
{
    Q_OBJECT

public:
             medAbstractDataMesh();
             medAbstractDataMesh(const  medAbstractDataMesh& other);
    virtual ~medAbstractDataMesh();

    void *mesh();

public:
    virtual int countVertices()
    { return 0; }
    virtual int countEdges()
    { return 0; }

    //    virtual QList<medAbstractDataVertex *> vertices();
    //    virtual QList<medAbstractDataEdge *> edges();
    

private:
    medAbstractDataMeshPrivate *d;
};


