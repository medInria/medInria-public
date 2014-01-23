/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

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
