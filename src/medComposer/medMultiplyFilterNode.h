// Copyright: INRIA

#pragma once

#include <dtkComposer>

#include "medAbstractImageData.h"
#include "medAbstractMultiplyFilter.h"

class medMultiplyFilterNodePrivate;

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

class medMultiplyFilterNode : public dtkComposerNodeObject<medAbstractMultiplyFilter>
{
public:
     medMultiplyFilterNode(void);
    ~medMultiplyFilterNode(void);

public:
    void run(void);

private:
    medMultiplyFilterNodePrivate *d;
};

//
// medBaseWriterNode.h ends here

