/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "manualRegistration.h"
#include "manualRegistrationPlugin.h"
#include "manualRegistrationToolBox.h"

manualRegistrationPlugin::manualRegistrationPlugin(QObject *parent) : medPluginLegacy(parent)
{
}

bool manualRegistrationPlugin::initialize()
{
    if(!manualRegistration::registered())
    {
        qWarning() << "Unable to register manualRegistration type";
    }
    
    if ( !manualRegistrationToolBox::registered() )
    {
        qWarning() << "Unable to register manualRegistration toolbox";
    }
    
    return true;
}

QString manualRegistrationPlugin::description() const
{
    QString description;
    description += "<h1>Manual Registration Tutorial</h1>";
    description += "Manual registration between two volumes using landmarks.";

    description += "<h2>Preparation</h2>";
    description += "<ul>";
    description += "<li>Drops 2 data in the view.</li>";
    description += "<li>Click on 'Start Manual Registration'.</li>";
    description += "<li>You can choose to perform a Rigid transformation algorithm, or an Affine one.</li>";
    description += "</ul>";

    description += "<h2>Landmarks</h2>";
    description += "You need to select some identical points of interest on both the data. ";
    description += "Firstly, choose a point of interest on the first image and add a landmark, then search the same point of interest on the second data and add a landmark on this second data.";
    description += "<ul>";
    description += "<li>To add a landmark: Shift + left mouse button.</li>";
    description += "<li>To remove a pair of landmarks: Backspace + left mouse button.</li>";
    description += "</ul>";

    description += "<h2>Run</h2>";
    description += "<ul>";
    description += "<li>When you are ready, click on 'Compute Registration'.</li>";
    description += "<li>The new transformation is added to the transformation list.</li>";
    description += "<li>You can redo, undo, reset, and export the last transformation to a file.</li>";
    description += "</ul>";
    description += "This plugin uses the ITK library: https://itk.org";
    return description;
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
