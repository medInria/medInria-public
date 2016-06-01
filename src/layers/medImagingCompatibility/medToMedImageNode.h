// Copyright: INRIA

#pragma once

#include <dtkComposer>

#include "medDataReaderWriter.h"
#include "medAbstractConverter.h"
#include <dtkImaging>

class medToMedImageNodePrivate;

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

class medToMedImageNode : public dtkComposerNodeObject<medAbstractConverter>
{
public:
     medToMedImageNode(void);
    ~medToMedImageNode(void);

public:
    void run(void);

private:
    medToMedImageNodePrivate *d;
};

//
// medBaseWriterNode.h ends here

