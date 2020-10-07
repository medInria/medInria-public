/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkCorePluginManager.h>
#include <dtkCore/dtkCorePlugin.h>

#include <medAbstractArea.h>

#include <medCoreExport.h>

class MEDCORE_EXPORT medAbstractAreaPlugin : public QObject
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

class MEDCORE_EXPORT medAbstractAreaPluginFactory : public dtkCorePluginFactory<medAbstractArea>
{
};

class MEDCORE_EXPORT medAbstractAreaPluginManager : public dtkCorePluginManager<medAbstractAreaPlugin>
{
};
