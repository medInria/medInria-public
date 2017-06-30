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
#include <undoRedoRegistrationToolBox.h>


bool undoRedoRegistrationPlugin::initialize(void)
{
    if (!undoRedoRegistration::registered())        { dtkWarn() << "Unable to register undoRedoRegistration type";    }
    if (!undoRedoRegistrationToolBox::registered()) { dtkWarn() << "Unable to register undoRedoRegistration toolbox"; }
    
    return true;
}

QString undoRedoRegistrationPlugin::name(void) const
{
    return "undoRedoRegistrationPlugin";
}

QString undoRedoRegistrationPlugin::version(void) const
{
    return UNDOREDOREGISTRATIONPLUGIN_VERSION;
}

QString undoRedoRegistrationPlugin::identifier(void) const
{
    return "undoRedoRegistration";
}

QStringList undoRedoRegistrationPlugin::types(void) const
{
    return QStringList() << "undoRedoRegistration";
}

Q_EXPORT_PLUGIN2(undoRedoRegistrationPlugin, undoRedoRegistrationPlugin)
