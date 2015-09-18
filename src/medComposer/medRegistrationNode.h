// Copyright: INRIA

#pragma once

#include <dtkComposer>

#include "medAbstractImageData.h"
#include "medAbstractRegistrationProcess.h"

class medRegistrationNodePrivate;

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

class medRegistrationNode : public dtkComposerNodeObject<medAbstractRegistrationProcess>
{
public:
     medRegistrationNode(void);
    ~medRegistrationNode(void);

public:
    void run(void);
    QWidget* editor();

private:
    medRegistrationNodePrivate *d;
    QWidget* m_widget;
};

//
// medBaseWriterNode.h ends here

