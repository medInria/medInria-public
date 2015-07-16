/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractAreaPlugin.h>

class medComposerAreaPlugin : public medAbstractAreaPlugin
{
    Q_OBJECT
    Q_INTERFACES(medAbstractAreaPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.medComposerAreaPlugin" FILE "medComposerAreaPlugin.json")

public:
     medComposerAreaPlugin(void) {}
    virtual ~medComposerAreaPlugin(void) {}

public:
    void initialize(void);
    void uninitialize(void);
};
