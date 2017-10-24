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

manualRegistrationPlugin::manualRegistrationPlugin(QObject *parent) : medPlugin(parent)
{
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

QString manualRegistrationPlugin::name() const
{
    return "Manual Registration";
}

QString manualRegistrationPlugin::version() const
{
    return MANUALREGISTRATIONPLUGIN_VERSION;
}

QStringList manualRegistrationPlugin::types() const
{
    return QStringList() << "manualRegistration";
}

Q_EXPORT_PLUGIN2(manualRegistrationPlugin, manualRegistrationPlugin)
