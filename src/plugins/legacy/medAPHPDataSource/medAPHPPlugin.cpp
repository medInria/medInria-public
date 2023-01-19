/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <medSourcesLoader.h>
#include "medAPHPPlugin.h"
#include "medAPHP.h"
#include "sphereDicomWeb/medAnnotation.h"
#include <PluginAPHP/QtDcmInterface.h>

medAPHPPlugin::medAPHPPlugin(QObject *parent) : medPluginLegacy(parent)
{
}

bool medAPHPPlugin::initialize()
{
    return medSourcesLoader::instance()->registerSourceType(
        "medAPHP",
        "Datasource de type APHP (PACS + APIRest)",
        "Ce type de datasource permet l'exploitation des données pour une installation de medInria dans l'environnement de l'EDS APHP",
        []() -> medAbstractSource* {return new medAPHP(QtDcmInterface::createNewInstance(), new medAnnotation()); });
}

QString medAPHPPlugin::name() const
{
    return "APHP Source == PACS+RestAPI";
}

QString medAPHPPlugin::description() const
{
    QString description = \
            tr("Fetch data from APHP PACS and store in EDS with RestAPI connection.");
    return description;
}

QString medAPHPPlugin::version() const
{
    return MEDAPHPPLUGIN_VERSION;
}

QStringList medAPHPPlugin::types() const
{
    return QStringList() << "medAPHP";
}
