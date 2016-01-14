// Copyright: INRIA

#pragma once

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

