// Copyright: INRIA

#pragma once

#include <dtkComposer>

#include "medAbstractImageData.h"
#include "medAbstractDilateFilter.h"

class medDilateFilterNodePrivate;

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

class medDilateFilterNode : public dtkComposerNodeObject<medAbstractDilateFilter>
{
public:
     medDilateFilterNode(void);
    ~medDilateFilterNode(void);

public:
    void run(void);

private:
    medDilateFilterNodePrivate *d;
};

//
// medBaseWriterNode.h ends here

