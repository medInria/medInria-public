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

#include <medComposerExport.h>

class MEDCOMPOSER_EXPORT medComposerFactoryExtension : public dtkComposerNodeFactoryExtension
{
public:
     medComposerFactoryExtension(void);
    ~medComposerFactoryExtension(void);

public:
    void extend(dtkComposerNodeFactory *factory);
};

//
// medComposerFactoryExtension.h ends here
