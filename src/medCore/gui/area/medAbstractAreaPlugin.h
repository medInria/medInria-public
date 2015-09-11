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

#include <medAbstractArea.h>

#include <medInriaExport.h>

class MEDINRIA_EXPORT medAbstractAreaPlugin : public QObject
{
    Q_OBJECT

public:
    medAbstractAreaPlugin(void)
    {

    }

    virtual ~medAbstractAreaPlugin(void)
    {

    }

public:
    virtual void initialize(void) = 0;
    virtual void uninitialize(void) = 0;
};

Q_DECLARE_INTERFACE(medAbstractAreaPlugin, DTK_DECLARE_PLUGIN_INTERFACE(medAbstractArea))

class MEDINRIA_EXPORT medAbstractAreaPluginFactory : public dtkCorePluginFactory<medAbstractArea>
{
};

class MEDINRIA_EXPORT medAbstractAreaPluginManager : public dtkCorePluginManager<medAbstractAreaPlugin>
{
};
