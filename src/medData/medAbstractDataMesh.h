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

#ifndef MEDABSTRACTMESHDATA_H
#define MEDABSTRACTMESHDATA_H

#include "medDataExport.h"
#include <medData/medAbstractData.h>

class medAbstractDataVertex;
class medAbstractDataEdge;
class medAbstractDataMeshPrivate;

class MEDDATA_EXPORT medAbstractDataMesh : public medAbstractData
{
    Q_OBJECT

public:
             medAbstractDataMesh(void);
             medAbstractDataMesh(const  medAbstractDataMesh& other);
    virtual ~medAbstractDataMesh(void);

    void *mesh(void);

public:
    virtual int countVertices(void)
    { return 0; }
    virtual int countEdges(void)
    { return 0; }

    //    virtual QList<medAbstractDataVertex *> vertices(void);
    //    virtual QList<medAbstractDataEdge *> edges(void);
    

private:
    medAbstractDataMeshPrivate *d;
};

#endif
