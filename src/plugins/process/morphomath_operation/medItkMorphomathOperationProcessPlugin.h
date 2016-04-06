/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractMorphomathOperationProcess.h>

#include <medItkMorphomathOperationProcessPluginExport.h>

class MEDITKMORPHOMATHOPERATIONPROCESSPLUGIN_EXPORT medItkMorphomathOperationProcessPlugin : public medAbstractMorphomathOperationProcessPlugin
{
    Q_OBJECT

    Q_INTERFACES(medAbstractMorphomathOperationProcessPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.medItkMorphomathOperationProcessPlugin" FILE "medItkMorphomathOperationProcessPlugin.json")

public:
     medItkMorphomathOperationProcessPlugin() {}
     virtual ~medItkMorphomathOperationProcessPlugin() {}

public:
    void initialize();
    void uninitialize();
};
