/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2017. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractBiasCorrectionProcess.h>

#include <medItkBiasCorrectionProcessPluginExport.h>

#include <medAbstractSingleFilterOperationProcess.h>

class MEDITKBIASCORRECTIONPROCESSPLUGIN_EXPORT medItkBiasCorrectionProcessPlugin : public medAbstractSingleFilterOperationProcessPlugin
{
    Q_OBJECT

    Q_INTERFACES(medAbstractSingleFilterOperationProcessPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.medItkBiasCorrectionProcessPlugin" FILE "medItkBiasCorrectionProcessPlugin.json")

public:
     medItkBiasCorrectionProcessPlugin() {}
     virtual ~medItkBiasCorrectionProcessPlugin() {}

public:
    void initialize();
    void uninitialize();
};
