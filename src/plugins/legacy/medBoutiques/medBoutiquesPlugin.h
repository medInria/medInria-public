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

#include <medBoutiquesPluginExport.h>

class MEDBOUTIQUESPLUGIN_EXPORT medBoutiquesPluginPrivate;

class MEDBOUTIQUESPLUGIN_EXPORT medBoutiquesPlugin : public dtkPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fr.inria.medBoutiquesPlugin" FILE "medBoutiquesPlugin.json")
    Q_INTERFACES(dtkPlugin)

public:
     medBoutiquesPlugin(QObject *parent = nullptr);
    ~medBoutiquesPlugin();

    virtual bool initialize();
    virtual bool uninitialize();

    virtual QString contact() const;
    virtual QString description() const;
    virtual QString name() const;
    virtual QString version() const;

    virtual QStringList tags() const;
    virtual QStringList types() const;

private:
    medBoutiquesPluginPrivate *d;
};


