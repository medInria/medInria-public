#pragma once
/*=========================================================================

medInria

Copyright (c) INRIA 2013 - 2020. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include <medPluginLegacy.h>

#include <medVtkViewPluginExport.h>

class MEDVTKVIEWPLUGIN_EXPORT medVtkViewPlugin : public medPluginLegacy
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fr.inria.medVtkViewPlugin" FILE "medVtkViewPlugin.json")
    Q_INTERFACES(dtkPlugin)

public:
     medVtkViewPlugin(QObject *parent = nullptr);

    virtual bool initialize();

    virtual QString name() const;
    virtual QString description() const;
    virtual QString version() const;
    virtual QStringList authors() const;
    virtual QString contact() const;
    virtual QStringList tags() const;
    virtual QStringList types() const;
};
