/* medAbstractDataMesh.cpp ---
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

#include <medAbstractDataMesh.h>
#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// medAbstractDataMeshPrivate
// /////////////////////////////////////////////////////////////////

class medAbstractDataMeshPrivate
{
public:
};

// /////////////////////////////////////////////////////////////////
// medAbstractDataMesh
// /////////////////////////////////////////////////////////////////

medAbstractDataMesh::medAbstractDataMesh(void) : medAbstractData(), d(new medAbstractDataMeshPrivate)
{

}

medAbstractDataMesh::medAbstractDataMesh(const  medAbstractDataMesh& other): medAbstractData(other), d(new medAbstractDataMeshPrivate)
{

}

medAbstractDataMesh::~medAbstractDataMesh(void)
{
    delete d;

    d = NULL;
}

void *medAbstractDataMesh::mesh(void)
{
    return this->data();
}
