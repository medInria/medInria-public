/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medMaskApplication.h"
#include "medMaskApplicationPlugin.h"
#include "medMaskApplicationToolBox.h"

medMaskApplicationPlugin::medMaskApplicationPlugin(QObject *parent) : medPlugin(parent)
{
}

bool medMaskApplicationPlugin::initialize()
{
    if(!medMaskApplication::registered())
    {
        dtkWarn() << "Unable to register medMaskApplication type";
    }
    
    if ( !medMaskApplicationToolBox::registered() )
    {
        dtkWarn() << "Unable to register medMaskApplication toolbox";
    }
    
    return true;
}

QString medMaskApplicationPlugin::description() const
{
    QString description = \
            "This plugin allows to apply a mask to a volume.<br>  \
            Drop your volume in the INPUT view, and your mask in the toolbox drop area.\
            <br><br>This process is based on the <a href=\"https://itk.org/\" style=\"color: #cc0000\" >ITK library</a>.";
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

Q_EXPORT_PLUGIN2(medMaskApplicationPlugin, medMaskApplicationPlugin)
