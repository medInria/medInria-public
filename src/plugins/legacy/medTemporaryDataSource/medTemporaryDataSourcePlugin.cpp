/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <medSourcesLoader.h>
#include "medTemporaryDataSourcePlugin.h"
#include "medTemporaryDataSource.h"

medTemporaryDataSourcePlugin::medTemporaryDataSourcePlugin(QObject *parent) : medPluginLegacy(parent)
{
}

medTemporaryDataSourcePlugin::~medTemporaryDataSourcePlugin()
{
}

bool medTemporaryDataSourcePlugin::initialize()
{
    return medDBSourcesLoader::instance()->registerSourceType(
        "medTemporaryDataSource",
        "Datasource de type temporaire",
        "Ce type de datasource permet l'exploitation des données en cache pendant l'exécution du logiciel",
        //&foo);
        []() -> medAbstractSource* {return new medTemporaryDataSource(); });
}

QString medTemporaryDataSourcePlugin::name() const
{
    return "Temporary Data Source";
}

QString medTemporaryDataSourcePlugin::description() const
{
    QString description = \
            tr("Store data in cache during the execution.");
    return description;
}

QString medTemporaryDataSourcePlugin::version() const
{
    return MEDTEMPORARYDATASOURCEPLUGIN_VERSION;
}

QStringList medTemporaryDataSourcePlugin::types() const
{
    return QStringList() << "medTemporaryDataSource";
}
