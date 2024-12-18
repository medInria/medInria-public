/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <medSourcesLoader.h>

#include <dcmtk/config/osconfig.h>

#include "medPACS.h"
#include "medPACSPlugin.h"

medPACSPlugin::medPACSPlugin(QObject *parent) : medPluginLegacy(parent)
{
}

medPACSPlugin::~medPACSPlugin()
{
}

bool medPACSPlugin::initialize()
{
    return medSourcesLoader::instance()->registerSourceType(
        "medPACS",
        "Datasource of type PACS",
        "This type of datasource enables communication with PACS servers using DCMTK or DICOMweb requests",
        //&foo);
        []() -> medAbstractSource* {return new medPACS(new dcmPACS()); });
}

QString medPACSPlugin::name() const
{
    return "PACS";
}

QString medPACSPlugin::description() const
{
    QString description = \
            tr("Fetch data from PACS.");
    return description;
}

QString medPACSPlugin::version() const
{
    return MEDPACSPLUGIN_VERSION;
}

QStringList medPACSPlugin::types() const
{
    return QStringList() << "medPACS";
}
