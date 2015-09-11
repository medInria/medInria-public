// Copyright: INRIA

#pragma once

#include <dtkComposer>

#include "medAbstractImageData.h"
#include "medAbstractErodeFilter.h"

class medErodeFilterNodePrivate;

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

class medErodeFilterNode : public dtkComposerNodeObject<medAbstractErodeFilter>
{
public:
     medErodeFilterNode(void);
    ~medErodeFilterNode(void);

public:
    void run(void);

private:
    medErodeFilterNodePrivate *d;
};

//
// medBaseWriterNode.h ends here

