/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.

 See LICENSE.txt for details in the root of the sources or:
 https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.

=========================================================================*/

#include "medExportVideo.h"
#include "medExportVideoPlugin.h"

namespace med
{

ExportVideoPlugin::ExportVideoPlugin(QObject *parent) : medPluginLegacy(parent)
{
}

bool ExportVideoPlugin::initialize()
{
    if (!ExportVideo::registered())
    {
        qDebug() << "Unable to register ExportVideo";
    }
    return true;
}

QString ExportVideoPlugin::name() const
{
    return "Export Video";
}

QString ExportVideoPlugin::description() const
{
    QString description = \
            tr("Export a 4D data as movie or JPEG images.\
               <br><br>This plugin uses the <a href=\"https://www.vtk.org/\" style=\"color: #cc0000\" >VTK library</a>.");
    return description;
}

QString ExportVideoPlugin::version() const
{
    return EXPORTVIDEOPLUGIN_VERSION;
}

QStringList ExportVideoPlugin::types() const
{
    return QStringList() << "ExportVideo";
}

}
