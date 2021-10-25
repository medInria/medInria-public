/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medMaskApplication.h"
#include "medMaskApplicationPlugin.h"
#include "medMaskApplicationToolBox.h"

medMaskApplicationPlugin::medMaskApplicationPlugin(QObject *parent) : medPluginLegacy(parent)
{
}

bool medMaskApplicationPlugin::initialize()
{
    if(!medMaskApplication::registered())
    {
        qWarning() << "Unable to register medMaskApplication type";
    }
    
    if(!medMaskApplicationToolBox::registered())
    {
        qWarning() << "Unable to register medMaskApplication toolbox";
    }
    
    return true;
}

QString medMaskApplicationPlugin::description() const
{
    QString description;
    description += "<h1>Mask Application Tutorial</h1>";
    description += "Apply a mask to a volume.";
    description += "<ul>";
    description += "<li>Drop a volume in the INPUT view.</li>";
    description += "<li>Drop a mask in the toolbox drop area.</li>";
    description += "<li>If needed, choose your mask background value.</li>";
    description += "<li>Click on 'Run'.</li>";
    description += "</ul>";
    description += "This plugin uses the ITK library: https://itk.org";
    return description;
}

QString medMaskApplicationPlugin::name() const
{
    return "Mask Application";
}

QString medMaskApplicationPlugin::version() const
{
    return MEDMASKAPPLICATIONPLUGIN_VERSION;
}

QStringList medMaskApplicationPlugin::types() const
{
    return QStringList() << "medMaskApplication";
}
