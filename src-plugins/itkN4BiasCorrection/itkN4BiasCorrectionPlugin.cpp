/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkN4BiasCorrection.h"
#include "itkN4BiasCorrectionPlugin.h"
#include "itkN4BiasCorrectionToolBox.h"

#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// itkN4BiasCorrectionPluginPrivate
// /////////////////////////////////////////////////////////////////

class itkN4BiasCorrectionPluginPrivate
{
public:
    // Class variables go here.
    static const char *s_Name;
};

const char * itkN4BiasCorrectionPluginPrivate::s_Name = "itkN4BiasCorrection";

// /////////////////////////////////////////////////////////////////
// itkN4BiasCorrectionPlugin
// /////////////////////////////////////////////////////////////////

itkN4BiasCorrectionPlugin::itkN4BiasCorrectionPlugin(QObject *parent) : dtkPlugin(parent), d(new itkN4BiasCorrectionPluginPrivate)
{
    
}

itkN4BiasCorrectionPlugin::~itkN4BiasCorrectionPlugin()
{
    delete d;
    
    d = NULL;
}

bool itkN4BiasCorrectionPlugin::initialize()
{
    if(!itkN4BiasCorrection::registered())
    {
        dtkWarn() << "Unable to register itkN4BiasCorrection type";
    }

    if ( !itkN4BiasCorrectionToolBox::registered() )
    {
        dtkWarn() << "Unable to register itkN4BiasCorrection toolbox";
    }
    return true;
}

bool itkN4BiasCorrectionPlugin::uninitialize()
{
    return true;
}

QString itkN4BiasCorrectionPlugin::name() const
{
    return "itkN4BiasCorrectionPlugin";
}

QString itkN4BiasCorrectionPlugin::description() const
{
    return tr("");
}

QString itkN4BiasCorrectionPlugin::version() const
{
    return ITKN4BIASCORRECTIONPLUGIN_VERSION;
}

QString itkN4BiasCorrectionPlugin::contact() const
{
    return "For any feedback <music-userfeedback@inria.fr>";
}

QStringList itkN4BiasCorrectionPlugin::authors() const
{
    return QStringList() << QString::fromUtf8("Loïc Cadour");
}

QStringList itkN4BiasCorrectionPlugin::contributors() const
{
    return QStringList() << "Mathilde Merle"
                         << "Florent Collot";
}

QString itkN4BiasCorrectionPlugin::identifier() const
{
    return itkN4BiasCorrectionPluginPrivate::s_Name;
}


QStringList itkN4BiasCorrectionPlugin::tags() const
{
    return QStringList();
}

QStringList itkN4BiasCorrectionPlugin::types() const
{
    return QStringList() << "itkN4BiasCorrection";
}
QStringList itkN4BiasCorrectionPlugin::dependencies() const
{
    return QStringList();
}
Q_EXPORT_PLUGIN2(itkN4BiasCorrectionPlugin, itkN4BiasCorrectionPlugin)
