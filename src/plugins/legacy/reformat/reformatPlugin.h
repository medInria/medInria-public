/*=========================================================================

 medInria

 Copyright (c) INRIA 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medPluginLegacy.h>
#include <reformatPluginExport.h>

class REFORMATPLUGIN_EXPORT reformatPlugin : public medPluginLegacy
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fr.inria.reformatPlugin" FILE "reformatPlugin.json")
    Q_INTERFACES(dtkPlugin)

public:
    reformatPlugin(QObject *parent = nullptr);
    virtual bool initialize();

    virtual QString description() const;
    virtual QString name() const;
    virtual QString version() const;
    virtual QStringList types() const;
};
