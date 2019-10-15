/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <voiCutterPlugin.h>
#include <voiCutterToolBox.h>

voiCutterPlugin::voiCutterPlugin(QObject *parent) : medPluginLegacy(parent)
{
}

bool voiCutterPlugin::initialize()
{
    if(!voiCutterToolBox::registered())
    {
        qDebug() << "Unable to register voiCutterToolBox";
    }
    return true;
}

QString voiCutterPlugin::name() const
{
    return "VOI Cutter";
}

QString voiCutterPlugin::description() const
{
    QString description = \
            "Segmentation of Volumes of Interest (VOI) on a volume or mask. \
            <br><br>This plugin uses the <a href=\"https://www.vtk.org/\" style=\"color: #cc0000\" >VTK library</a>.";
    return description;
}

QString voiCutterPlugin::version() const
{
    return VOICUTTERPLUGIN_VERSION;
}

