/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCoreSupport/dtkPlugin.h>

#include "LCCLogDemonsPluginExport.h"

class LCCLogDemonsPLUGIN_EXPORT LCCLogDemonsPluginPrivate;

class LCCLogDemonsPLUGIN_EXPORT LCCLogDemonsPlugin : public dtkPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fr.inria.LCCLogDemonsPlugin" FILE "LCCLogDemonsPlugin.json")
    Q_INTERFACES(dtkPlugin)
    
public:
    LCCLogDemonsPlugin(QObject *parent = 0);
    ~LCCLogDemonsPlugin();
    
    virtual bool initialize();
    virtual bool uninitialize();
    
    virtual QString name() const;
    virtual QString identifier() const;
    virtual QString description() const;
    virtual QString contact() const;
    virtual QString version() const;
    virtual QStringList authors() const;
    virtual QStringList contributors() const;
    virtual QStringList dependencies() const;
    
    virtual QStringList tags() const;
    virtual QStringList types() const;
    
private:
    LCCLogDemonsPluginPrivate *d;
};
