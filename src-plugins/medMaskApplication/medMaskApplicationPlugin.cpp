/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medMaskApplication.h"
#include "medMaskApplicationPlugin.h"
#include "medMaskApplicationToolBox.h"

#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// medMaskApplicationPluginPrivate
// /////////////////////////////////////////////////////////////////

class medMaskApplicationPluginPrivate
{
public:
    // Class variables go here.
    static const char *s_Name;
};

const char * medMaskApplicationPluginPrivate::s_Name = "medMaskApplication";

// /////////////////////////////////////////////////////////////////
// medMaskApplicationPlugin
// /////////////////////////////////////////////////////////////////

medMaskApplicationPlugin::medMaskApplicationPlugin(QObject *parent) : dtkPlugin(parent), d(new medMaskApplicationPluginPrivate)
{
    
}

medMaskApplicationPlugin::~medMaskApplicationPlugin()
{
    delete d;
    
    d = NULL;
}

bool medMaskApplicationPlugin::initialize()
{
    if(!medMaskApplication::registered())
    {
        dtkWarn() << "Unable to register medMaskApplication type";
    }
    
    if ( !medMaskApplicationToolBox::registered() )
    {
        dtkWarn() << "Unable to register medMaskApplication toolbox";
    }
    
    return true;
}

bool medMaskApplicationPlugin::uninitialize()
{
    return true;
}

QString medMaskApplicationPlugin::name() const
{
    return "medMaskApplicationPlugin";
}

QString medMaskApplicationPlugin::description() const
{
    return tr("");
}

QString medMaskApplicationPlugin::version() const
{
    return MEDMASKAPPLICATIONPLUGIN_VERSION;
}

QString medMaskApplicationPlugin::contact() const
{
    return "For any feedback <music-userfeedback@inria.fr>";
}

QStringList medMaskApplicationPlugin::authors() const
{
    QStringList list;
    return list;
}

QStringList medMaskApplicationPlugin::contributors() const
{
    return QStringList() << QString::fromUtf8("Loïc Cadour")
                         << "Mathilde Merle"
                         << "Florent Collot";
}

QString medMaskApplicationPlugin::identifier() const
{
    return medMaskApplicationPluginPrivate::s_Name;
}


QStringList medMaskApplicationPlugin::tags() const
{
    return QStringList();
}

QStringList medMaskApplicationPlugin::types() const
{
    return QStringList() << "medMaskApplication";
}
QStringList medMaskApplicationPlugin::dependencies() const
{
    return QStringList();
}
Q_EXPORT_PLUGIN2(medMaskApplicationPlugin, medMaskApplicationPlugin)
