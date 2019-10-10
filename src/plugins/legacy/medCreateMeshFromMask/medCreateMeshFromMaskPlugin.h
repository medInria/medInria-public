/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medCreateMeshFromMaskPluginExport.h>
#include <medPluginLegacy.h>

class MEDCREATEMESHFROMMASKPLUGIN_EXPORT medCreateMeshFromMaskPlugin : public medPluginLegacy
{
    Q_OBJECT
    Q_INTERFACES(dtkPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.medCreateMeshFromMaskPlugin" FILE "medCreateMeshFromMaskPlugin.json")
    
public:
    medCreateMeshFromMaskPlugin(QObject *parent = nullptr);
    virtual bool initialize();

    virtual QString description() const;
    virtual QString name() const;
    virtual QString version() const;
    virtual QStringList types() const;
};


