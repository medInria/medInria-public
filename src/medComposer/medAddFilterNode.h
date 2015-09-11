// Copyright: INRIA

#pragma once

#include <dtkComposer>

#include "medAbstractImageData.h"
#include "medAbstractAddFilter.h"

class medAddFilterNodePrivate;

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

class medAddFilterNode : public dtkComposerNodeObject<medAbstractAddFilter>
{
public:
     medAddFilterNode(void);
    ~medAddFilterNode(void);

public:
    void run(void);

private:
    medAddFilterNodePrivate *d;
};

//
// medBaseWriterNode.h ends here

