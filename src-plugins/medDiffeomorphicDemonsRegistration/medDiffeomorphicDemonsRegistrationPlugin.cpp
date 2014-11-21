/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDiffeomorphicDemonsRegistrationProcess.h>
#include <medDiffeomorphicDemonsRegistrationPlugin.h>

#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// medDiffeomorphicDemonsRegistrationPluginPrivate
// /////////////////////////////////////////////////////////////////

class medDiffeomorphicDemonsRegistrationPluginPrivate
{
public:
    // Class variables go here.
};

// /////////////////////////////////////////////////////////////////
// medDiffeomorphicDemonsRegistrationPlugin
// /////////////////////////////////////////////////////////////////

medDiffeomorphicDemonsRegistrationPlugin::medDiffeomorphicDemonsRegistrationPlugin(QObject *parent) : dtkPlugin(parent), d(new medDiffeomorphicDemonsRegistrationPluginPrivate)
{

}

medDiffeomorphicDemonsRegistrationPlugin::~medDiffeomorphicDemonsRegistrationPlugin()
{
    delete d;

    d = NULL;
}

bool medDiffeomorphicDemonsRegistrationPlugin::initialize()
{
    if (!medDiffeomorphicDemonsRegistrationProcess::registered())        { dtkWarn() << "Unable to register medDiffeomorphicDemonsRegistrationProcess type";    }

    return true;
}

bool medDiffeomorphicDemonsRegistrationPlugin::uninitialize()
{
    return true;
}

QString medDiffeomorphicDemonsRegistrationPlugin::name() const
{
    return "medDiffeomorphicDemonsRegistrationPlugin";
}

QString medDiffeomorphicDemonsRegistrationPlugin::contact() const
{
    return QString::fromUtf8("benoit.bleuze@inria.fr");
}

QStringList medDiffeomorphicDemonsRegistrationPlugin::authors() const
{
    QStringList list;
    list << QString::fromUtf8("Benoît Bleuzé");
    return list;
}

QStringList medDiffeomorphicDemonsRegistrationPlugin::contributors() const
{
    QStringList list;
    list <<  "Vincent Garcia";
    return list;
}

QString medDiffeomorphicDemonsRegistrationPlugin::description() const
{
    return tr("Applies the diffeomorphic demons as they can be found in itk. Converts any type of image to float before applying the change, since the diffeomorphic demons only work on float images <br/> see: <a href=\"http://www.insight-journal.org/browse/publication/154\" > http://www.insight-journal.org/browse/publication/154 </a>");
}


QString medDiffeomorphicDemonsRegistrationPlugin::version() const
{
    return MEDDIFFEOMORPHICDEMONSREGISTRATIONPLUGIN_VERSION;
}

QStringList medDiffeomorphicDemonsRegistrationPlugin::dependencies() const
{
    return QStringList();
}


QStringList medDiffeomorphicDemonsRegistrationPlugin::tags() const
{
    return QStringList();
}


QStringList medDiffeomorphicDemonsRegistrationPlugin::types() const
{
    return QStringList() << "medDiffeomorphicDemonsRegistrationProcess";
}

Q_EXPORT_PLUGIN2(medDiffeomorphicDemonsRegistrationPlugin, medDiffeomorphicDemonsRegistrationPlugin)


