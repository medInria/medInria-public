/*=========================================================================

 medInria

 Copyright (c) INRIA 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medCropToolBox.h>
#include <medReformatWorkspace.h>

#include <reformatPlugin.h>
#include <resliceToolBox.h>
#include <resampleProcess.h>

reformatPlugin::reformatPlugin(QObject *parent) : medPluginLegacy(parent)
{
}

bool reformatPlugin::initialize()
{
    if(!medReformatWorkspace::registered())
    {
        qWarning() << "Unable to register medReformatWorkspace type";
    }
    if (!resliceToolBox::registered())
    {
        qWarning() << "Unable to register resliceToolBox type";
    }
    if (!medCropToolBox::registered())
    {
        qWarning() << "Unable to register medCropToolBox type";
    }
    if (!resampleProcess::registered())
    {
        qWarning() << "Unable to register resample process";
    }
    return true;
}

QString reformatPlugin::description() const
{
  QString description = \
          "This plugin implements two toolboxes: \
          <br><h5>Cropping</h5><br>  \
          Crop a volume into a smaller one. \
          <br><h5>Reslice</h5><br>  \
          Reorient a volume, change its spacing or dimension. \
          <br><br>This plugin uses the <a href=\"https://itk.org/\" style=\"color: #cc0000\" >ITK</a> \
          and <a href=\"https://www.vtk.org/\" style=\"color: #cc0000\" >VTK</a> libraries.";
  return description;
}

QString reformatPlugin::name() const
{
    return "Reformat";
}

QString reformatPlugin::version() const
{
    return REFORMATPLUGIN_VERSION;
}

QStringList reformatPlugin::types() const
{
    return QStringList() << "medReformatWorkspace" << "medCropToolBox" << "resliceToolBox" << "resampleProcess";
}
