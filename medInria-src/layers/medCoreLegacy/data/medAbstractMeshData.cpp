/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractMeshData.h>
#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// medAbstractMeshDataPrivate
// /////////////////////////////////////////////////////////////////

class medAbstractMeshDataPrivate
{
public:
};

// /////////////////////////////////////////////////////////////////
// medAbstractMeshData
// /////////////////////////////////////////////////////////////////

medAbstractMeshData::medAbstractMeshData(void) : medAbstractData(), d(new medAbstractMeshDataPrivate)
{
}

medAbstractMeshData::medAbstractMeshData(const  medAbstractMeshData& other): medAbstractData(other), d(new medAbstractMeshDataPrivate)
{
}

medAbstractMeshData::~medAbstractMeshData(void)
{
    delete d;
    d = NULL;
}

void *medAbstractMeshData::mesh(void)
{
    return this->data();
}
