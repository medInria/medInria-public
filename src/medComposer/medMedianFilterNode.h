// Copyright: INRIA

#pragma once

#include <dtkComposer>

#include "medAbstractImageData.h"
#include "medAbstractMedianFilter.h"

class medMedianFilterNodePrivate;

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

class medMedianFilterNode : public dtkComposerNodeObject<medAbstractMedianFilter>
{
public:
     medMedianFilterNode(void);
    ~medMedianFilterNode(void);

public:
    void run(void);

private:
    medMedianFilterNodePrivate *d;
};

//
// medBaseWriterNode.h ends here

