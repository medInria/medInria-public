/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medPluginLegacy.h>

#include "LCCLogDemonsPluginExport.h"

class LCCLogDemonsPLUGIN_EXPORT LCCLogDemonsPlugin : public medPluginLegacy
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fr.inria.LCCLogDemonsPlugin" FILE "LCCLogDemonsPlugin.json")
    Q_INTERFACES(dtkPlugin)
    
public:
    LCCLogDemonsPlugin(QObject *parent = nullptr);
    
    virtual bool initialize();
    
    virtual QString name() const;
    virtual QString description() const;
    virtual QStringList types() const;
};
