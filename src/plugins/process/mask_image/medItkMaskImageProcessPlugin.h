/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractMaskImageProcess.h>

#include <medItkMaskImageProcessPluginExport.h>

class MEDITKMASKIMAGEPROCESSPLUGIN_EXPORT medItkMaskImageProcessPlugin : public medAbstractMaskImageProcessPlugin
{
    Q_OBJECT

    Q_INTERFACES(medAbstractMaskImageProcessPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.medItkMaskImageProcessPlugin" FILE "medItkMaskImageProcessPlugin.json")

public:
     medItkMaskImageProcessPlugin() {}
     virtual ~medItkMaskImageProcessPlugin() {}

public:
    void initialize();
    void uninitialize();
};
