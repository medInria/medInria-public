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
    QString description;
    description += "<h1>VOI Cutter Tutorial</h1>";
    description += "Drop a volume in the view.<br>";

    description += "<h2>Rotation</h2>";
    description += " You can rotate the data holding your mouse cursor over it. Display an area to remove.";

    description += "<h2>Segment</h2>";
    description += "When you are ready, click on 'Cut Volume' and ";
    description += "draw a segmentation by clicking on the view. <br>";

    description += "<h2>Cut</h2>";
    description += "If you press 'Backspace', the projected area will be removed from the mask.<br>";
    description += "If you press 'Enter' this area will be kept and the outside removed.";

    description += "<h2>Save</h2>";
    description += " Click on 'Save Image' to save your new volume.";

    description += "<br><br>This plugin uses the VTK library: https://vtk.org";
    return description;
}

QString voiCutterPlugin::version() const
{
    return VOICUTTERPLUGIN_VERSION;
}

