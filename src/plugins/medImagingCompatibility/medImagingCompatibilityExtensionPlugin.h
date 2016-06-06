/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkComposer/dtkComposerExtension.h>

#include <QtCore>

class medImagingCompatibilityExtensionPlugin : public dtkComposerExtensionPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkComposerExtensionPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.medImagingCompatibilityExtensionPlugin" FILE "medImagingCompatibilityExtensionPlugin.json")

public:
     medImagingCompatibilityExtensionPlugin(void) {}
    ~medImagingCompatibilityExtensionPlugin(void) {}

public:
    void   initialize(void);
    void uninitialize(void);
};
