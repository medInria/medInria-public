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

#include <medAbstractAddImageProcess.h>

class medAbstractAddImageProcessPlugin : public QObject
{
    Q_OBJECT

public:
    medAbstractAddImageProcessPlugin(void)
    {
    }

    virtual ~medAbstractAddImageProcessPlugin(void)
    {
    }

public:
    virtual void initialize(void) = 0;
    virtual void uninitialize(void) = 0;
};

Q_DECLARE_INTERFACE(medAbstractAddImageProcessPlugin, DTK_DECLARE_PLUGIN_INTERFACE(medAbstractAddImageProcess))

class medAbstractAddImageProcessPluginFactory : public dtkCorePluginFactory<medAbstractAddImageProcess>
{
};

class medAbstractAddImageProcessPluginManager : public dtkCorePluginManager<medAbstractAddImageProcessPlugin>
{
};
