/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkProcessRegistrationDiffeomorphicDemons.h>
#include <itkProcessRegistrationDiffeomorphicDemonsPlugin.h>

#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// itkProcessRegistrationDiffeomorphicDemonsPluginPrivate
// /////////////////////////////////////////////////////////////////

class itkProcessRegistrationDiffeomorphicDemonsPluginPrivate
{
public:
    // Class variables go here.
};

// /////////////////////////////////////////////////////////////////
// itkProcessRegistrationDiffeomorphicDemonsPlugin
// /////////////////////////////////////////////////////////////////

itkProcessRegistrationDiffeomorphicDemonsPlugin::itkProcessRegistrationDiffeomorphicDemonsPlugin(QObject *parent) : dtkPlugin(parent), d(new itkProcessRegistrationDiffeomorphicDemonsPluginPrivate)
{

}

itkProcessRegistrationDiffeomorphicDemonsPlugin::~itkProcessRegistrationDiffeomorphicDemonsPlugin()
{
    delete d;

    d = NULL;
}

bool itkProcessRegistrationDiffeomorphicDemonsPlugin::initialize()
{
    if (!itkProcessRegistrationDiffeomorphicDemons::registered())        { dtkWarn() << "Unable to register itkProcessRegistrationDiffeomorphicDemons type";    }

    return true;
}

bool itkProcessRegistrationDiffeomorphicDemonsPlugin::uninitialize()
{
    return true;
}

QString itkProcessRegistrationDiffeomorphicDemonsPlugin::name() const
{
    return "itkProcessRegistrationDiffeomorphicDemonsPlugin";
}

QString itkProcessRegistrationDiffeomorphicDemonsPlugin::contact() const
{
    return QString::fromUtf8("benoit.bleuze@inria.fr");
}

QStringList itkProcessRegistrationDiffeomorphicDemonsPlugin::authors() const
{
    QStringList list;
    list << QString::fromUtf8("Benoît Bleuzé");
    return list;
}

QStringList itkProcessRegistrationDiffeomorphicDemonsPlugin::contributors() const
{
    QStringList list;
    list <<  "Vincent Garcia";
    return list;
}

QString itkProcessRegistrationDiffeomorphicDemonsPlugin::description() const
{
    return tr("Applies the diffeomorphic demons as they can be found in itk. Converts any type of image to float before applying the change, since the diffeomorphic demons only work on float images <br/> see: <a href=\"http://www.insight-journal.org/browse/publication/154\" > http://www.insight-journal.org/browse/publication/154 </a>");
}


QString itkProcessRegistrationDiffeomorphicDemonsPlugin::version() const
{
    return ITKPROCESSREGISTRATIONDIFFEOMORPHICDEMONSPLUGIN_VERSION;
}

QStringList itkProcessRegistrationDiffeomorphicDemonsPlugin::dependencies() const
{
    return QStringList();
}


QStringList itkProcessRegistrationDiffeomorphicDemonsPlugin::tags() const
{
    return QStringList();
}


QStringList itkProcessRegistrationDiffeomorphicDemonsPlugin::types() const
{
    return QStringList() << "itkProcessRegistrationDiffeomorphicDemons";
}

Q_EXPORT_PLUGIN2(itkProcessRegistrationDiffeomorphicDemonsPlugin, itkProcessRegistrationDiffeomorphicDemonsPlugin)


