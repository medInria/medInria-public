// Copyright: INRIA

#pragma once

#include <dtkComposer>

#include "medAbstractImageData.h"
#include "medAbstractGaussianFilter.h"

class medGaussianFilterNodePrivate;

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

class medGaussianFilterNode : public dtkComposerNodeObject<medAbstractGaussianFilter>
{
public:
     medGaussianFilterNode(void);
    ~medGaussianFilterNode(void);

public:
    void run(void);

private:
    medGaussianFilterNodePrivate *d;
};

//
// medBaseWriterNode.h ends here

