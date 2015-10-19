// Copyright: INRIA

#pragma once

#include <dtkComposer>

class medGetEnvVariableNodePrivate;

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

class medGetEnvVariableNode : public dtkComposerNodeLeaf
{
public:
     medGetEnvVariableNode(void);
    ~medGetEnvVariableNode(void);

public:
    void run(void);

private:
    medGetEnvVariableNodePrivate *d;
};

//
// medBaseWriterNode.h ends here

