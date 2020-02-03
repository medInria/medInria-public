#pragma once
/*=========================================================================

medInria

Copyright (c) INRIA 2013 - 2020. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include <dtkCoreSupport/dtkPlugin.h>
#include <medVtkFibersDataPluginExport.h>

class medVtkFibersDataPluginPrivate;

class MEDVTKFIBERSDATAPLUGIN_EXPORT medVtkFibersDataPlugin : public dtkPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fr.inria.medVtkFibersDataPlugin" FILE "medVtkFibersDataPlugin.json")
    Q_INTERFACES(dtkPlugin)

public:
     medVtkFibersDataPlugin(QObject *parent = nullptr);
    ~medVtkFibersDataPlugin();

    virtual bool initialize();
    virtual bool uninitialize();

    virtual QString name() const;
    virtual QString description() const;
    virtual QString version() const;

    virtual QStringList tags() const;
    virtual QStringList types() const;

private:
     medVtkFibersDataPluginPrivate *d;
};
