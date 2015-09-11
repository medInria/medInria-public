/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractOpeningImageProcess.h>

#include <medPluginExport.h>

class MEDPLUGIN_EXPORT medItkOpeningImageProcessPlugin : public medAbstractOpeningImageProcessPlugin
{
    Q_OBJECT
    Q_INTERFACES(medAbstractOpeningImageProcessPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.medItkOpeningImageProcessPlugin" FILE "medItkOpeningImageProcessPlugin.json")

public:
     medItkOpeningImageProcessPlugin(void) {}
    virtual ~medItkOpeningImageProcessPlugin(void) {}

public:
    void initialize(void);
    void uninitialize(void);
};
