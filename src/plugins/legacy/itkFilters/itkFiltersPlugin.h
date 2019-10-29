/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <itkFiltersPluginExport.h>

#include <medPluginLegacy.h>

class ITKFILTERSPLUGIN_EXPORT itkFiltersPlugin : public medPluginLegacy
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fr.inria.itkFiltersPlugin" FILE "itkFiltersPlugin.json")
    Q_INTERFACES(dtkPlugin)

public:
    itkFiltersPlugin(QObject *parent = nullptr);
    virtual bool initialize();

    virtual QString description() const;
    virtual QString name() const;
    virtual QString version() const;
    virtual QStringList tags() const;
    virtual QStringList types() const;
};
