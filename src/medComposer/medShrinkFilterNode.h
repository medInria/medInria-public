// Copyright: INRIA

#pragma once

#include <dtkComposer>

#include "medAbstractImageData.h"
#include "medAbstractShrinkFilter.h"

class medShrinkFilterNodePrivate;

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

class medShrinkFilterNode : public dtkComposerNodeObject<medAbstractShrinkFilter>
{
public:
     medShrinkFilterNode(void);
    ~medShrinkFilterNode(void);

public:
    void run(void);

private:
    medShrinkFilterNodePrivate *d;
};

//
// medBaseWriterNode.h ends here

