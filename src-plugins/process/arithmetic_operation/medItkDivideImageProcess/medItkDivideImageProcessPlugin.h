/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractDivideImageProcess.h>

#include <medPluginExport.h>

class MEDPLUGIN_EXPORT medItkDivideImageProcessPlugin : public medAbstractDivideImageProcessPlugin
{
    Q_OBJECT
    Q_INTERFACES(medAbstractDivideImageProcessPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.medItkDivideImageProcessPlugin" FILE "medItkDivideImageProcessPlugin.json")

public:
     medItkDivideImageProcessPlugin(void) {}
    virtual ~medItkDivideImageProcessPlugin(void) {}

public:
    void initialize(void);
    void uninitialize(void);
};
