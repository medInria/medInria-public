/* medAbstractDataMesh.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Feb  2 09:08:09 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Feb  1 22:19:21 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 43
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

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


