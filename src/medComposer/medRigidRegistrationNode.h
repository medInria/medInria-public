// Copyright: INRIA

#pragma once

#include <dtkComposer>

#include "medAbstractImageData.h"
#include "medAbstractRigidRegistration.h"

class medRigidRegistrationNodePrivate;

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

class medRigidRegistrationNode : public dtkComposerNodeObject<medAbstractRigidRegistration>
{
public:
     medRigidRegistrationNode(void);
    ~medRigidRegistrationNode(void);

public:
    void run(void);

private:
    medRigidRegistrationNodePrivate *d;
};

//
// medBaseWriterNode.h ends here

