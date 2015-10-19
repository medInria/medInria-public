// Copyright: INRIA

#pragma once

#include <dtkComposer>

#include "medAbstractImageData.h"
#include "medAbstractWindowingFilter.h"

class medWindowingFilterNodePrivate;

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

class medWindowingFilterNode : public dtkComposerNodeObject<medAbstractWindowingFilter>
{
public:
     medWindowingFilterNode(void);
    ~medWindowingFilterNode(void);

public:
    void run(void);

private:
    medWindowingFilterNodePrivate *d;
};

//
// medBaseWriterNode.h ends here

