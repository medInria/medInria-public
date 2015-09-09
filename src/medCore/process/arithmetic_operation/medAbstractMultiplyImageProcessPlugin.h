/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkCorePluginManager.h>
#include <dtkCore/dtkCorePlugin.h>

#include <medAbstractMultiplyImageProcess.h>

class medAbstractMultiplyImageProcessPlugin : public QObject
{
    Q_OBJECT

public:
    medAbstractMultiplyImageProcessPlugin(void)
    {
    }

    virtual ~medAbstractMultiplyImageProcessPlugin(void)
    {
    }

public:
    virtual void initialize(void) = 0;
    virtual void uninitialize(void) = 0;
};

Q_DECLARE_INTERFACE(medAbstractMultiplyImageProcessPlugin, DTK_DECLARE_PLUGIN_INTERFACE(medAbstractMultiplyImageProcess))

class medAbstractMultiplyImageProcessPluginFactory : public dtkCorePluginFactory<medAbstractMultiplyImageProcess>
{
};

class medAbstractMultiplyImageProcessPluginManager : public dtkCorePluginManager<medAbstractMultiplyImageProcessPlugin>
{
};
