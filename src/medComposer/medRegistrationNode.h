// Copyright: INRIA

#pragma once

#include <dtkComposer>

#include "medAbstractImageData.h"
#include "medAbstractRigidRegistration.h"

class medRegistrationNodePrivate;

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

class medRegistrationNode : public dtkComposerNodeObject<medAbstractRigidRegistration>
{
public:
     medRegistrationNode(void);
    ~medRegistrationNode(void);

public:
    void run(void);

private:
    medRegistrationNodePrivate *d;
};

//
// medBaseWriterNode.h ends here

