// Version: $Id$
// 
// 

// Commentary: 
// 
// 

// Change Log:
// 
// 

// Code:

#pragma once

#include <dtkComposer/dtkComposerNodeFactory.h>

class medComposerFactoryExtension : public dtkComposerNodeFactoryExtension
{
public:
     medComposerFactoryExtension(void);
    ~medComposerFactoryExtension(void);

public:
    void extend(dtkComposerNodeFactory *factory);
};

// 
// medComposerFactoryExtension.h ends here
