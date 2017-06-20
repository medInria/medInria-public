/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <manualRegistration.h>
#include <manualRegistrationPlugin.h>
#include <manualRegistrationToolBox.h>

#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// manualRegistrationPluginPrivate
// /////////////////////////////////////////////////////////////////

class manualRegistrationPluginPrivate
{
public:
    // Class variables go here.
    static const char *s_Name;
};

const char * manualRegistrationPluginPrivate::s_Name = "manualRegistration";

// /////////////////////////////////////////////////////////////////
// manualRegistrationPlugin
// /////////////////////////////////////////////////////////////////

manualRegistrationPlugin::manualRegistrationPlugin(QObject *parent) : dtkPlugin(parent), d(new manualRegistrationPluginPrivate)
{
    
}

manualRegistrationPlugin::~manualRegistrationPlugin()
{
    delete d;
    
    d = NULL;
}

bool manualRegistrationPlugin::initialize()
{
    if(!manualRegistration::registered())
    {
        dtkWarn() << "Unable to register manualRegistration type";
    }
    
    if ( !manualRegistrationToolBox::registered() )
    {
        dtkWarn() << "Unable to register manualRegistration toolbox";
    }
    
    return true;
}

bool manualRegistrationPlugin::uninitialize()
{
    return true;
}

QString manualRegistrationPlugin::name() const
{
    return "manualRegistrationPlugin";
}

QString manualRegistrationPlugin::description() const
{
    return tr("");
}

QString manualRegistrationPlugin::version() const
{
    return MANUALREGISTRATIONPLUGIN_VERSION;
}

QString manualRegistrationPlugin::contact() const
{
    return "For any feedback <music-userfeedback@inria.fr>";
}

QStringList manualRegistrationPlugin::authors() const
{
    QStringList list;
    return list;
}

QStringList manualRegistrationPlugin::contributors() const
{
    QStringList list;
    list << "Mathilde Merle";
    list << QString::fromUtf8("Loïc Cadour");
    list << "Florent Collot";
    return list;
}

QString manualRegistrationPlugin::identifier() const
{
    return manualRegistrationPluginPrivate::s_Name;
}


QStringList manualRegistrationPlugin::tags() const
{
    return QStringList();
}

QStringList manualRegistrationPlugin::types() const
{
    return QStringList() << "manualRegistration";
}
QStringList manualRegistrationPlugin::dependencies() const
{
    return QStringList();
}
Q_EXPORT_PLUGIN2(manualRegistrationPlugin, manualRegistrationPlugin)
