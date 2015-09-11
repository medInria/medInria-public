// Copyright: INRIA

#pragma once

#include <dtkComposer>

#include "medAbstractImageData.h"
#include "medAbstractOpenFilter.h"

class medOpenFilterNodePrivate;

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

class medOpenFilterNode : public dtkComposerNodeObject<medAbstractOpenFilter>
{
public:
     medOpenFilterNode(void);
    ~medOpenFilterNode(void);

public:
    void run(void);

private:
    medOpenFilterNodePrivate *d;
};

//
// medBaseWriterNode.h ends here

