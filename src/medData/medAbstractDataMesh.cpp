/* dtkAbstractDataMesh.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Feb  2 09:08:09 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Feb  1 22:17:52 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 65
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include <dtkCore/dtkAbstractDataMesh.h>
#include <dtkCore/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// dtkAbstractDataMeshPrivate
// /////////////////////////////////////////////////////////////////

class dtkAbstractDataMeshPrivate
{
public:
};

// /////////////////////////////////////////////////////////////////
// dtkAbstractDataMesh
// /////////////////////////////////////////////////////////////////

dtkAbstractDataMesh::dtkAbstractDataMesh(void) : dtkAbstractData(), d(new dtkAbstractDataMeshPrivate)
{

}

dtkAbstractDataMesh::dtkAbstractDataMesh(const  dtkAbstractDataMesh& other): dtkAbstractData(other), d(new dtkAbstractDataMeshPrivate)
{

}

dtkAbstractDataMesh::~dtkAbstractDataMesh(void)
{
    delete d;

    d = NULL;
}

void *dtkAbstractDataMesh::mesh(void)
{
    return this->data();
}
