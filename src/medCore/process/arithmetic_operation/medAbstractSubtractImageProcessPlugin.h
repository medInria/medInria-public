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

#include <medAbstractSubtractImageProcess.h>

class medAbstractSubtractImageProcessPlugin : public QObject
{
    Q_OBJECT

public:
    medAbstractSubtractImageProcessPlugin(void)
    {
    }

    virtual ~medAbstractSubtractImageProcessPlugin(void)
    {
    }

public:
    virtual void initialize(void) = 0;
    virtual void uninitialize(void) = 0;
};

Q_DECLARE_INTERFACE(medAbstractSubtractImageProcessPlugin, DTK_DECLARE_PLUGIN_INTERFACE(medAbstractSubtractImageProcess))

class medAbstractSubtractImageProcessPluginFactory : public dtkCorePluginFactory<medAbstractSubtractImageProcess>
{
};

class medAbstractSubtractImageProcessPluginManager : public dtkCorePluginManager<medAbstractSubtractImageProcessPlugin>
{
};
