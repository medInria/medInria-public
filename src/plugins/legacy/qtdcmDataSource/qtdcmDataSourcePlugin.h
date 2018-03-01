/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCoreSupport/dtkPlugin.h>

#include <qtdcmDataSourcePluginExport.h>

class QTDCMDATASOURCEPLUGIN_EXPORT qtdcmDataSourcePluginPrivate;

class QTDCMDATASOURCEPLUGIN_EXPORT qtdcmDataSourcePlugin : public dtkPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fr.inria.qtdcmDataSourcePlugin" FILE "qtdcmDataSourcePlugin.json")
    Q_INTERFACES(dtkPlugin)

public:
     qtdcmDataSourcePlugin(QObject *parent = 0);
    ~qtdcmDataSourcePlugin();

    virtual bool initialize();
    virtual bool uninitialize();

    virtual QString contact() const;
    virtual QString description() const;
    virtual QString name() const;
    virtual QString version() const;
    virtual QStringList authors() const;
    virtual QStringList contributors() const;
    virtual QStringList dependencies() const;

    virtual QStringList tags() const;
    virtual QStringList types() const;

private:
     qtdcmDataSourcePluginPrivate *d;
};


