#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractData.h>

class medAbstractMeshDataPrivate;

class MEDCORE_EXPORT medAbstractMeshData : public medAbstractData
{
    Q_OBJECT

public:
    medAbstractMeshData();
    medAbstractMeshData(const  medAbstractMeshData& other);
    virtual ~medAbstractMeshData();

    void *mesh();

public:
    virtual int countVertices(){ return 0; }
    virtual int countEdges(){ return 0; }

private:
    medAbstractMeshDataPrivate *d;
};


