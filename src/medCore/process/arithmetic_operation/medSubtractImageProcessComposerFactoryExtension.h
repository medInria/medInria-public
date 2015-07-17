/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkComposer/dtkComposerNodeFactory.h>

class medSubtractImageProcessComposerFactoryExtension : public dtkComposerNodeFactoryExtension
{
public:
     medSubtractImageProcessComposerFactoryExtension(void);
     virtual ~medSubtractImageProcessComposerFactoryExtension(void);

public:
    void extend(dtkComposerNodeFactory *factory);
};

//
// dtkLinearAlgebraSparseComposerFactoryExtension.h ends here
