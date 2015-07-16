/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medComposerAreaPlugin.h>

#include <medGuiLayer.h>
#include <medComposerArea.h>


void medComposerAreaPlugin::initialize(void)
{
    medGuiLayer::area::pluginFactory().record("medComposerArea",
                                              medComposerAreaCreator);
}

void medComposerAreaPlugin::uninitialize(void)
{

}

// ///////////////////////////////////////////////////////////////////
// Plugin meta data
// ///////////////////////////////////////////////////////////////////

DTK_DEFINE_PLUGIN(medItkSubtractImageProcess)
