// Copyright: INRIA

#pragma once

#include <dtkComposer>

#include "medDataReaderWriter.h"
#include "medAbstractConverter.h"
#include <dtkImaging>

class medToDtkImageNodePrivate;

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

class medToDtkImageNode : public dtkComposerNodeObject<medAbstractConverter>
{
public:
     medToDtkImageNode(void);
    ~medToDtkImageNode(void);

public:
    void run(void);

private:
    medToDtkImageNodePrivate *d;
};

//
// medBaseWriterNode.h ends here

