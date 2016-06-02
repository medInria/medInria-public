/* dtkImagingCompatibilityExtensionPlugin.h ---
 *
 * Author: Nicolas Niclausse
 * Copyright (C) Inria.
 */

#pragma once

#include <dtkComposer/dtkComposerExtension.h>

#include <QtCore>

class dtkImagingCompatibilityExtensionPlugin : public dtkComposerExtensionPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkComposerExtensionPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.dtkImagingCompatibilityExtensionPlugin" FILE "dtkImagingCompatibilityExtensionPlugin.json")

public:
     dtkImagingCompatibilityExtensionPlugin(void) {}
    ~dtkImagingCompatibilityExtensionPlugin(void) {}

public:
    void   initialize(void);
    void uninitialize(void);
};
