/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkPlugin.h>

#include <medVtkViewPluginExport.h>

class medVtkViewPluginPrivate;

class MEDVTKVIEWPLUGIN_EXPORT medVtkViewPlugin : public dtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkPlugin)

public:
     medVtkViewPlugin(QObject *parent = 0);
    ~medVtkViewPlugin();

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
     medVtkViewPluginPrivate *d;
};


