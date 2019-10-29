/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "diffeomorphicDemonsPluginExport.h"

#include <medPluginLegacy.h>

class DIFFEOMORPHICDEMONSPLUGIN_EXPORT diffeomorphicDemonsPluginPrivate;

class DIFFEOMORPHICDEMONSPLUGIN_EXPORT diffeomorphicDemonsPlugin : public medPluginLegacy
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fr.inria.diffeomorphicDemonsPlugin" FILE "diffeomorphicDemonsPlugin.json")
    Q_INTERFACES(dtkPlugin)

public:
    diffeomorphicDemonsPlugin(QObject *parent = nullptr);

    virtual bool initialize();

    virtual QString contact() const;
    virtual QString description() const;
    virtual QString name() const;
    virtual QString version() const;

    virtual QStringList authors() const;
    virtual QStringList contributors() const;
    virtual QStringList types() const;
};
