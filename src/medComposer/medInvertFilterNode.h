// Copyright: INRIA

#pragma once

#include <dtkComposer>

#include "medAbstractImageData.h"
#include "medAbstractInvertFilter.h"

class medInvertFilterNodePrivate;

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

class medInvertFilterNode : public dtkComposerNodeObject<medAbstractInvertFilter>
{
public:
     medInvertFilterNode(void);
    ~medInvertFilterNode(void);

public:
    void run(void);

private:
    medInvertFilterNodePrivate *d;
};

//
// medBaseWriterNode.h ends here

