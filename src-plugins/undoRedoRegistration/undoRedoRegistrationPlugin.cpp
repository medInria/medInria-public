/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <undoRedoRegistration.h>
#include <undoRedoRegistrationPlugin.h>

#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// undoRedoRegistrationPluginPrivate
// /////////////////////////////////////////////////////////////////

class undoRedoRegistrationPluginPrivate
{
public:
    // Class variables go here.
    static const char *s_Name;
};

const char * undoRedoRegistrationPluginPrivate::s_Name = "undoRedoRegistration";

// /////////////////////////////////////////////////////////////////
// undoRedoRegistrationPlugin
// /////////////////////////////////////////////////////////////////

undoRedoRegistrationPlugin::undoRedoRegistrationPlugin(QObject *parent) : dtkPlugin(parent), d(new undoRedoRegistrationPluginPrivate)
{
    
}

undoRedoRegistrationPlugin::~undoRedoRegistrationPlugin(void)
{
    delete d;
    
    d = NULL;
}

bool undoRedoRegistrationPlugin::initialize(void)
{
    //if (!undoRedoRegistration::registered())        { dtkWarn() << "Unable to register undoRedoRegistration type";    }
    
    return true;
}

bool undoRedoRegistrationPlugin::uninitialize(void)
{
    return true;
}

QString undoRedoRegistrationPlugin::name(void) const
{
    return "undoRedoRegistrationPlugin";
}

QString undoRedoRegistrationPlugin::description(void) const
{
    return tr("");
}

QString undoRedoRegistrationPlugin::version(void) const
{
    return UNDOREDOREGISTRATIONPLUGIN_VERSION;
}

QString undoRedoRegistrationPlugin::contact(void) const
{
    return "";
}

QStringList undoRedoRegistrationPlugin::authors(void) const
{
    QStringList list;
    return list;
}

QStringList undoRedoRegistrationPlugin::contributors(void) const
{
    QStringList list;
    return list;
}

QString undoRedoRegistrationPlugin::identifier(void) const
{
    return undoRedoRegistrationPluginPrivate::s_Name;
}


QStringList undoRedoRegistrationPlugin::tags(void) const
{
    return QStringList();
}

QStringList undoRedoRegistrationPlugin::types(void) const
{
    return QStringList() << "undoRedoRegistration";
}
QStringList undoRedoRegistrationPlugin::dependencies(void) const
{
    return QStringList();
}

Q_EXPORT_PLUGIN2(undoRedoRegistrationPlugin, undoRedoRegistrationPlugin)
