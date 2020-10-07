/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkComposer/dtkComposerNodeFactory.h>
#include <dtkComposer/dtkComposerExtension.h>

#include <medComposerImagingExtensionExport.h>

class MEDCOMPOSERIMAGINGEXTENSION_EXPORT medComposerImagingExtension : public dtkComposerExtension
{
public:
     medComposerImagingExtension(void);
    ~medComposerImagingExtension(void);

public:
    void extend(dtkComposerNodeFactory *factory);
};

//
// medComposerExtension.h ends here
