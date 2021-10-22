/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include "itkProcessRegistrationOptimus.h"
#include "itkProcessRegistrationOptimusPlugin.h"
#include "itkProcessRegistrationOptimusToolBox.h"

// /////////////////////////////////////////////////////////////////
// itkProcessRegistrationOptimusPlugin
// /////////////////////////////////////////////////////////////////

itkProcessRegistrationOptimusPlugin::itkProcessRegistrationOptimusPlugin(QObject *parent) : medPluginLegacy(parent)
{

}

bool itkProcessRegistrationOptimusPlugin::initialize()
{
    if(!itkProcessRegistrationOptimus::registered())
    {
        qWarning() << "Unable to register itkProcessRegistrationOptimus type";
    }

    if(!itkProcessRegistrationOptimusToolBox::registered())
    {
        qWarning() << "Unable to register itkProcessRegistrationOptimus toolbox";
    }

    return true;
}

QString itkProcessRegistrationOptimusPlugin::name() const
{
    return "itkProcessRegistrationOptimusPlugin";
}

QString itkProcessRegistrationOptimusPlugin::description() const
{
    QString description;
    description += "<h1>Optimus Registration Tutorial</h1>";
    description += "This method is based on the New UOA optimizer. The metric used is the mutual information. ";
    description += "The transformation computed is a rigid transformation.";
    description += "<ul>";
    description += "<li>Drops 2 data in the view.</li>";
    description += "<li>Select your parameters.</li>";
    description += "<li>When you are ready, click on 'Run'.</li>";
    description += "<li>The new transformation is added to the transformation list.</li>";
    description += "<li>You can redo, undo, reset, and export the last transformation to a file.</li>";
    description += "</ul>";
    return description;
}

QStringList itkProcessRegistrationOptimusPlugin::authors() const
{
    QStringList list;
    list << "Olivier Clatz";
    return list;
}

QStringList itkProcessRegistrationOptimusPlugin::contributors() const
{
    QStringList list;
    list <<  QString::fromUtf8("Benoît Bleuzé")
          << "Vincent Garcia";
    return list;
}

QString itkProcessRegistrationOptimusPlugin::version() const
{
    return ITKPROCESSREGISTRATIONOPTIMUSPLUGIN_VERSION;
}

QStringList itkProcessRegistrationOptimusPlugin::types() const
{
    return QStringList() << "itkProcessRegistrationOptimus";
}
