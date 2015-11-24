/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkPlugin.h>

#include "reformatPluginExport.h"

class REFORMATPLUGIN_EXPORT reformatPluginPrivate;

class REFORMATPLUGIN_EXPORT reformatPlugin : public dtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkPlugin)
    
public:
    reformatPlugin(QObject *parent = 0);
    ~reformatPlugin();
    
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
    reformatPluginPrivate *d;
};


