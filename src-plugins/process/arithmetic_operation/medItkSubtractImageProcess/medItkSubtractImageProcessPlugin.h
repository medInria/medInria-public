/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractSubtractImageProcess.h>

#include <medPluginExport.h>

class MEDPLUGIN_EXPORT medItkSubtractImageProcessPlugin : public medAbstractSubtractImageProcessPlugin
{
    Q_OBJECT
    Q_INTERFACES(medAbstractSubtractImageProcessPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.medItkSubtractImageProcessPlugin" FILE "medItkSubtractImageProcessPlugin.json")

public:
     medItkSubtractImageProcessPlugin(void) {}
    virtual ~medItkSubtractImageProcessPlugin(void) {}

public:
    void initialize(void);
    void uninitialize(void);
};
