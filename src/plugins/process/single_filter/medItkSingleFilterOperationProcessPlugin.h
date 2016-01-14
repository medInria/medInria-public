/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractSingleFilterOperationProcess.h>

#include <medItkSingleFilterOperationProcessPluginExport.h>

class MEDITKSINGLEFILTEROPERATIONPROCESSPLUGINS_EXPORTS medItkSingleFilterOperationProcessPlugin : public medAbstractSingleFilterOperationProcessPlugin
{
    Q_OBJECT

    Q_INTERFACES(medAbstractSingleFilterOperationProcessPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.medAbstractSingleFilterOperationProcessPlugin" FILE "medAbstractSingleFilterOperationProcessPlugin.json")

public:
     medItkSingleFilterOperationProcessPlugin() {}
     virtual ~medItkSingleFilterOperationProcessPlugin() {}

public:
    void initialize();
    void uninitialize();
};
