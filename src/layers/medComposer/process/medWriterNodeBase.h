#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkComposer>

#include "medDataReaderWriter.h"
#include "medAbstractData.h"
#include "medAbstractImageData.h"
#include "medAbstractMeshData.h"

class medWriterNodeBasePrivate;

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

class medWriterNodeBase : public dtkComposerNodeLeaf
{
public:
     medWriterNodeBase(void);
    ~medWriterNodeBase(void);

public:
    void run(void);

private:
    medWriterNodeBasePrivate *d;
};

//
// medBaseWriterNode.h ends here

