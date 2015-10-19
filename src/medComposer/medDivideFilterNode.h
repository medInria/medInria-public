// Copyright: INRIA

#pragma once

#include <dtkComposer>

#include "medAbstractImageData.h"
#include "medAbstractDivideFilter.h"

class medDivideFilterNodePrivate;

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

class medDivideFilterNode : public dtkComposerNodeObject<medAbstractDivideFilter>
{
public:
     medDivideFilterNode(void);
    ~medDivideFilterNode(void);

public:
    void run(void);

private:
    medDivideFilterNodePrivate *d;
};

//
// medBaseWriterNode.h ends here

