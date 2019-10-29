/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medN4BiasCorrectionPluginExport.h>
#include <medPluginLegacy.h>

class MEDN4BIASCORRECTIONPLUGIN_EXPORT medN4BiasCorrectionPlugin : public medPluginLegacy
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fr.inria.medN4BiasCorrectionPlugin" FILE "medN4BiasCorrectionPlugin.json")
    Q_INTERFACES(dtkPlugin)

public:
    medN4BiasCorrectionPlugin(QObject *parent = nullptr);
    virtual bool initialize();
    
    virtual QString description() const;
    virtual QString name() const;
    virtual QString version() const;
    virtual QStringList types() const;
};
