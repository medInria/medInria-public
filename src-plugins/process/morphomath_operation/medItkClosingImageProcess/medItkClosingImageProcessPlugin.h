/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractClosingImageProcess.h>

class MEDINRIA_EXPORT medItkClosingImageProcessPlugin : public medAbstractClosingImageProcessPlugin
{
    Q_OBJECT
    Q_INTERFACES(medAbstractClosingImageProcessPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.medItkClosingImageProcessPlugin" FILE "medItkClosingImageProcessPlugin.json")

public:
     medItkClosingImageProcessPlugin(void) {}
    virtual ~medItkClosingImageProcessPlugin(void) {}

public:
    void initialize(void);
    void uninitialize(void);
};
