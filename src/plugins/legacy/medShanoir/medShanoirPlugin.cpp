/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <medSourcesLoader.h>
#include <QObject>
#include "medShanoirPlugin.h"
#include "medShanoir.h"

medShanoirPlugin::medShanoirPlugin(QObject *parent) : medPluginLegacy(parent)
{
}

medShanoirPlugin::~medShanoirPlugin()
{
}

bool medShanoirPlugin::initialize()
{
    return medSourcesLoader::instance()->registerSourceType(
        "medShanoir",
        "Datasource of type Shanoir",
        "This type of datasource allows to handle data from Shanoir"
        []() -> medAbstractSource* {
		medShanoir *shanoirInstance = new medShanoir();
		return shanoirInstance;
	});
}

QString medShanoirPlugin::name() const
{
    return "Shanoir server source";
}

QString medShanoirPlugin::description() const
{
    QString description = \
            tr("Synchronize Shanoir with medInria : retrieve all the studies you are working with.");
    return description;
}

QString medShanoirPlugin::version() const
{
    return SHANOIRPLUGIN_VERSION;
}

QStringList medShanoirPlugin::types() const
{
    return QStringList() << "medShanoir";
}
