/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractAddImageProcess.h>

#include <medPluginExport.h>

class MEDPLUGIN_EXPORT medItkAddImageProcessPlugin : public medAbstractAddImageProcessPlugin
{
    Q_OBJECT
    Q_INTERFACES(medAbstractAddImageProcessPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.medItkAddImageProcessPlugin" FILE "medItkAddImageProcessPlugin.json")

public:
     medItkAddImageProcessPlugin(void) {}
    virtual ~medItkAddImageProcessPlugin(void) {}

public:
    void initialize(void);
    void uninitialize(void);
};
