/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "LCCLogDemons.h"
#include "LCCLogDemonsPlugin.h"
#include "LCCLogDemonsToolBox.h"

#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// LCCLogDemonsPluginPrivate
// /////////////////////////////////////////////////////////////////

class LCCLogDemonsPluginPrivate
{
public:
    // Class variables go here.
    static const char *s_Name;
};

const char * LCCLogDemonsPluginPrivate::s_Name = "LCCLogDemons";

// /////////////////////////////////////////////////////////////////
// LCCLogDemonsPlugin
// /////////////////////////////////////////////////////////////////

LCCLogDemonsPlugin::LCCLogDemonsPlugin(QObject *parent) : dtkPlugin(parent), d(new LCCLogDemonsPluginPrivate)
{
    
}

LCCLogDemonsPlugin::~LCCLogDemonsPlugin()
{
    delete d;
    
    d = NULL;
}

bool LCCLogDemonsPlugin::initialize()
{
    if(!LCCLogDemons::registered()) {
        dtkWarn() << "Unable to register LCCLogDemons type";
    }
    
    if(!LCCLogDemonsToolBox::registered()) {
        dtkWarn() << "Unable to register LCCLogDemons toolbox";
    }
    
    return true;
}

bool LCCLogDemonsPlugin::uninitialize()
{
    return true;
}

QString LCCLogDemonsPlugin::name() const
{
    return "LCCLogDemonsPlugin";
}

QString LCCLogDemonsPlugin::description() const
{
    return tr("LCC Log Demons");
}

QString LCCLogDemonsPlugin::version() const
{
    return tr("3.1.1");
}

QString LCCLogDemonsPlugin::contact() const
{
    return "";
}

QStringList LCCLogDemonsPlugin::authors() const
{
    QStringList list;
    return list;
}

QStringList LCCLogDemonsPlugin::contributors() const
{
    QStringList list;
    return list;
}

QString LCCLogDemonsPlugin::identifier() const
{
    return LCCLogDemonsPluginPrivate::s_Name;
}


QStringList LCCLogDemonsPlugin::tags() const
{
    return QStringList();
}

QStringList LCCLogDemonsPlugin::types() const
{
    return QStringList() << "LCCLogDemons";
}

QStringList LCCLogDemonsPlugin::dependencies() const
{
    return QStringList();
}
