/* dtkImagingComposerExtension.cpp ---
 *
 * Author: Nicolas Niclausse
 * Copyright (C) Inria.
 */
//
//


// Code:

#include "dtkImagingCompatibilityExtensionPlugin.h"
#include "medComposerImagingExtension.h"
#include "dtkImaging.h"

#include <dtkComposer>
#include <dtkLog>

// ///////////////////////////////////////////////////////////////////
// Helper functions
// ///////////////////////////////////////////////////////////////////

dtkComposerExtension* dtkImagingCompatibilityCreator(void);

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////


void dtkImagingCompatibilityExtensionPlugin::initialize(void)
{
    dtkComposer::extension::pluginFactory().record("dtkImagingCompatibility", dtkImagingCompatibilityCreator);
    dtkComposerExtension *extension = dtkComposer::extension::pluginFactory().create("dtkImagingCompatibility");
    bool verbose = dtkComposer::extension::pluginManager().verboseLoading();
    dtkImaging::setVerboseLoading(verbose);
    extension->extend(&(dtkComposer::node::factory()));
}

void dtkImagingCompatibilityExtensionPlugin::uninitialize(void)
{

}


// ///////////////////////////////////////////////////////////////////
// Plugin meta data
// ///////////////////////////////////////////////////////////////////

DTK_DEFINE_PLUGIN(dtkComposerExtension)

// ///////////////////////////////////////////////////////////////////
// Helper functions
// ///////////////////////////////////////////////////////////////////

dtkComposerExtension *dtkImagingCompatibilityCreator(void)
{
    return new medComposerImagingExtension();
}
