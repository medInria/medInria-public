// Copyright: INRIA

#pragma once

#include <dtkComposer>
#include "medAbstractSegmentationProcess.h"

class medSegmentationNodePrivate;

// ///////////////////////////////////////////////////////////////////
// medSegmentation
// ///////////////////////////////////////////////////////////////////

class medSegmentationNode : public dtkComposerNodeObject<medAbstractSegmentationProcess>
{
public:
     medSegmentationNode(void);
    ~medSegmentationNode(void);

public:
    void run(void);


private:
    medSegmentationNodePrivate *d;
};
