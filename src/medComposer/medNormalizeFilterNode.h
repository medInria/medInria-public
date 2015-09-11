// Copyright: INRIA

#pragma once

#include <dtkComposer>

#include "medAbstractImageData.h"
#include "medAbstractNormalizeFilter.h"

class medNormalizeFilterNodePrivate;

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

class medNormalizeFilterNode : public dtkComposerNodeObject<medAbstractNormalizeFilter>
{
public:
     medNormalizeFilterNode(void);
    ~medNormalizeFilterNode(void);

public:
    void run(void);

private:
    medNormalizeFilterNodePrivate *d;
};

//
// medBaseWriterNode.h ends here

