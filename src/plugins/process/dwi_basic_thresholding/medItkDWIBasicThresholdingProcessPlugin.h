/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractDWIMaskingProcess.h>
#include <medItkDWIBasicThresholdingProcessPluginExport.h>

class MEDITKDWIBASICTHRESHOLDINGPROCESSPLUGIN_EXPORT medItkDWIBasicThresholdingProcessPlugin : public medAbstractDWIMaskingProcessPlugin
{
    Q_OBJECT

    Q_INTERFACES(medAbstractDWIMaskingProcessPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.medItkDWIBasicThresholdingProcessPlugin" FILE "medItkDWIBasicThresholdingProcessPlugin.json")

public:
     medItkDWIBasicThresholdingProcessPlugin() {}
     virtual ~medItkDWIBasicThresholdingProcessPlugin() {}

public:
    void initialize();
    void uninitialize();
};
