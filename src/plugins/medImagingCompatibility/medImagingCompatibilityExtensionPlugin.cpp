/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medImagingCompatibilityExtensionPlugin.h"
#include "medComposerImagingExtension.h"
#include "dtkImaging.h"

#include <dtkComposer>
#include <dtkLog>

// ///////////////////////////////////////////////////////////////////
// Helper functions
// ///////////////////////////////////////////////////////////////////

dtkComposerExtension* medImagingCompatibilityCreator(void);

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////


void medImagingCompatibilityExtensionPlugin::initialize(void)
{
    dtkComposer::extension::pluginFactory().record("medImagingCompatibility", medImagingCompatibilityCreator);
    dtkComposerExtension *extension = dtkComposer::extension::pluginFactory().create("medImagingCompatibility");
    bool verbose = dtkComposer::extension::pluginManager().verboseLoading();
    dtkImaging::setVerboseLoading(verbose);
    extension->extend(&(dtkComposer::node::factory()));
}

void medImagingCompatibilityExtensionPlugin::uninitialize(void)
{

}

// ///////////////////////////////////////////////////////////////////
// Plugin meta data
// ///////////////////////////////////////////////////////////////////

DTK_DEFINE_PLUGIN(dtkComposerExtension)

// ///////////////////////////////////////////////////////////////////
// Helper functions
// ///////////////////////////////////////////////////////////////////

dtkComposerExtension *medImagingCompatibilityCreator(void)
{
    return new medComposerImagingExtension();
}
