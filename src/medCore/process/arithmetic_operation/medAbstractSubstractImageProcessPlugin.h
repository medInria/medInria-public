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

#include <medAbstractSubstractImageProcessPresenter.h>

class medAbstractSubstractImageProcessPlugin : public QObject
{
    Q_OBJECT

public:
    medAbstractSubstractImageProcessPlugin(void)
    {

    }

    virtual ~medAbstractSubstractImageProcessPlugin(void)
    {

    }

public:
    virtual void initialize(void) = 0;
    virtual void uninitialize(void) = 0;
};

Q_DECLARE_INTERFACE(medAbstractSubstractImageProcessPlugin, DTK_DECLARE_PLUGIN_INTERFACE(medAbstractSubstractImageProcessPresenter))

class medAbstractSubstractImageProcessPluginFactory : public dtkCorePluginFactory<medAbstractSubstractImageProcessPresenter>
{
};

class medAbstractSubstractImageProcessPluginManager : public dtkCorePluginManager<medAbstractSubstractImageProcessPlugin>
{
};
