// Copyright: INRIA

#pragma once

#include <dtkComposer>

class SpacerNodePrivate;

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

class SpacerNode : public dtkComposerNodeLeaf
{
public:
     SpacerNode(void);
    ~SpacerNode(void);

public:
    void run(void);

private:
    SpacerNodePrivate *d;
};

//
// medBaseWriterNode.h ends here

