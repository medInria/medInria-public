/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractArithmeticOperationProcess.h>

#include <medPluginExport.h>

class MEDPLUGINS_EXPORT medItkArithmeticOperationProcessPlugin : public medAbstractArithmeticOperationProcessPlugin
{
    Q_OBJECT

    Q_INTERFACES(medAbstractArithmeticOperationProcessPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.medItkArithmeticOperationProcessPlugin" FILE "medItkArithmeticOperationProcessPlugin.json")

public:
     medItkArithmeticOperationProcessPlugin() {}
     virtual ~medItkArithmeticOperationProcessPlugin() {}

public:
    void initialize();
    void uninitialize();
};
