// Copyright: INRIA

#pragma once

#include <dtkComposer>

class medFSLInitNodePrivate;

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

class medFSLInitNode : public dtkComposerNodeLeaf
{
public:
     medFSLInitNode(void);
    ~medFSLInitNode(void);

public:
    void run(void);

private:
    medFSLInitNodePrivate *d;
};

//
// medBaseWriterNode.h ends here

