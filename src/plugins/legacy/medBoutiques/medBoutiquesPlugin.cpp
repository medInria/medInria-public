/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medBoutiques.h>
#include <medBoutiquesPlugin.h>
#include <medBoutiquesToolBox.h>
#include <medBoutiquesDefinitions.h>

#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// medBoutiquesPluginPrivate
// /////////////////////////////////////////////////////////////////

class medBoutiquesPluginPrivate
{
public:
    // Class variables go here.
};

// /////////////////////////////////////////////////////////////////
// medBoutiquesPlugin
// /////////////////////////////////////////////////////////////////

medBoutiquesPlugin::medBoutiquesPlugin(QObject *parent) : medPluginLegacy(parent), d(new medBoutiquesPluginPrivate)
{

}

medBoutiquesPlugin::~medBoutiquesPlugin()
{
    delete d;

    d = nullptr;
}

bool medBoutiquesPlugin::initialize()
{
    if(!medBoutiques::registered())
    {
        dtkWarn() << "Unable to register medBoutiques type";
    }

    if(!medBoutiquesToolBox::registered())
    {
        dtkWarn() << "Unable to register medBoutiques toolbox";
    }

    return true;
}

bool medBoutiquesPlugin::uninitialize()
{
    return true;
}

QString medBoutiquesPlugin::name() const
{
    return "medBoutiquesPlugin";
}

QString medBoutiquesPlugin::contact() const
{
    return QString::fromUtf8("Arthur Masson arthur.masson@inria.fr");
}

QString medBoutiquesPlugin::version() const
{
    return PLUGIN_VERSION;
}

QString medBoutiquesPlugin::description() const
{
    return "Interface to use Boutiques tools in medInria";
}

QStringList medBoutiquesPlugin::tags() const
{
    return QStringList() << "Boutiques" << "process";
}

QStringList medBoutiquesPlugin::types() const
{
    return QStringList() << "medBoutiques";
}
