/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractDilateImageProcess.h>

class MEDINRIA_EXPORT medItkDilateImageProcessPlugin : public medAbstractDilateImageProcessPlugin
{
    Q_OBJECT
    Q_INTERFACES(medAbstractDilateImageProcessPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.medItkDilateImageProcessPlugin" FILE "medItkDilateImageProcessPlugin.json")

public:
     medItkDilateImageProcessPlugin(void) {}
    virtual ~medItkDilateImageProcessPlugin(void) {}

public:
    void initialize(void);
    void uninitialize(void);
};
