// Copyright: INRIA

#pragma once

#include <dtkComposer>

#include "medAbstractImageData.h"
#include "medAbstractSubstractFilter.h"

class medSubstractFilterNodePrivate;

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

class medSubstractFilterNode : public dtkComposerNodeObject<medAbstractSubstractFilter>
{
public:
     medSubstractFilterNode(void);
    ~medSubstractFilterNode(void);

public:
    void run(void);

private:
    medSubstractFilterNodePrivate *d;
};

//
// medBaseWriterNode.h ends here

