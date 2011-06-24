/* dtkAbstractDataMesh.h --- 
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

#ifndef DTKABSTRACTMESHDATA_H
#define DTKABSTRACTMESHDATA_H

#include <dtkCore/dtkAbstractData.h>

class dtkAbstractDataVertex;
class dtkAbstractDataEdge;
class dtkAbstractDataMeshPrivate;

class DTKCORE_EXPORT dtkAbstractDataMesh : public dtkAbstractData
{
    Q_OBJECT

public:
             dtkAbstractDataMesh(void);
             dtkAbstractDataMesh(const  dtkAbstractDataMesh& other);
    virtual ~dtkAbstractDataMesh(void);

    void *mesh(void);

public:
    virtual int countVertices(void)
    { return 0; }
    virtual int countEdges(void)
    { return 0; }

    //    virtual QList<dtkAbstractDataVertex *> vertices(void);
    //    virtual QList<dtkAbstractDataEdge *> edges(void);
    

private:
    dtkAbstractDataMeshPrivate *d;
};

#endif
