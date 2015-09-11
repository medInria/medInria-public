// Copyright: INRIA

#pragma once

#include <dtkComposer>

#include "medAbstractImageData.h"
#include "medAbstractCloseFilter.h"

class medCloseFilterNodePrivate;

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

class medCloseFilterNode : public dtkComposerNodeObject<medAbstractCloseFilter>
{
public:
     medCloseFilterNode(void);
    ~medCloseFilterNode(void);

public:
    void run(void);

private:
    medCloseFilterNodePrivate *d;
};

//
// medBaseWriterNode.h ends here

