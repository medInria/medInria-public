/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractMultiplyImageProcess.h>

class medItkMultiplyImageProcessPlugin : public medAbstractMultiplyImageProcessPlugin
{
    Q_OBJECT
    Q_INTERFACES(medAbstractMultiplyImageProcessPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.medItkMultiplyImageProcessPlugin" FILE "medItkMultiplyImageProcessPlugin.json")

public:
     medItkMultiplyImageProcessPlugin(void) {}
    virtual ~medItkMultiplyImageProcessPlugin(void) {}

public:
    void initialize(void);
    void uninitialize(void);
};
