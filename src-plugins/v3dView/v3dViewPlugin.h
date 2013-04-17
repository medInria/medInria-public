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

#include "v3dViewPluginExport.h"

class V3DVIEWPLUGIN_EXPORT v3dViewPluginPrivate;

class V3DVIEWPLUGIN_EXPORT v3dViewPlugin : public dtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkPlugin)

public:
     v3dViewPlugin(QObject *parent = 0);
    ~v3dViewPlugin();

    virtual bool initialize();
    virtual bool uninitialize();

    virtual QString name() const;
    virtual QString description() const;
    virtual QString version() const;

    virtual QStringList authors() const;
    virtual QString contact() const;
    virtual QStringList contributors() const;


    virtual QStringList tags() const;
    virtual QStringList types() const;

private:
     v3dViewPluginPrivate *d;
};


