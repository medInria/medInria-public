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

// /////////////////////////////////////////////////////////////////
// LCCLogDemonsPlugin
// /////////////////////////////////////////////////////////////////

LCCLogDemonsPlugin::LCCLogDemonsPlugin(QObject *parent) 
: medPluginLegacy(parent)
{
    
}

bool LCCLogDemonsPlugin::initialize()
{
    if(!LCCLogDemons::registered())
    {
        qWarning() << "Unable to register LCCLogDemons type";
    }
    
    if(!LCCLogDemonsToolBox::registered())
    {
        qWarning() << "Unable to register LCCLogDemons toolbox";
    }
    
    return true;
}

QString LCCLogDemonsPlugin::name() const
{
    return "LCCLogDemonsPlugin";
}

QString LCCLogDemonsPlugin::description() const
{
    QString description;
    description += "<h1>LCC Log Demons Tutorial</h1>";
    description += "Apply LCC log demons algorithm on 2 images.<br>";
    description += "<ul>";
    description += "<li>Drops 2 data with the same size and spacing in the view.</li>";
    description += "<li>Select your parameters.</li>";
    description += "<li>When you are ready, click on 'Run'.</li>";
    description += "<li>The new transformation is added to the transformation list.</li>";
    description += "<li>You can redo, undo, reset, and export the last transformation to a file.</li>";
    description += "</ul>";
    return description;
}

QStringList LCCLogDemonsPlugin::types() const
{
    return QStringList() << "LCCLogDemons";
}
