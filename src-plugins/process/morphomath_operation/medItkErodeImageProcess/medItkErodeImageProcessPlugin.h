/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractErodeImageProcess.h>

class MEDINRIA_EXPORT medItkErodeImageProcessPlugin : public medAbstractErodeImageProcessPlugin
{
    Q_OBJECT
    Q_INTERFACES(medAbstractErodeImageProcessPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.medItkErodeImageProcessPlugin" FILE "medItkErodeImageProcessPlugin.json")

public:
     medItkErodeImageProcessPlugin(void) {}
    virtual ~medItkErodeImageProcessPlugin(void) {}

public:
    void initialize(void);
    void uninitialize(void);
};
