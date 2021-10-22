/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "diffeomorphicDemons.h"
#include "diffeomorphicDemonsPlugin.h"
#include "diffeomorphicDemonsToolBox.h"

// /////////////////////////////////////////////////////////////////
// diffeomorphicDemonsPlugin
// /////////////////////////////////////////////////////////////////

diffeomorphicDemonsPlugin::diffeomorphicDemonsPlugin(QObject *parent):
    medPluginLegacy(parent)
{
}

bool diffeomorphicDemonsPlugin::initialize()
{
    if (!diffeomorphicDemons::registered())
    {
        qWarning() << "Unable to register diffeomorphicDemons type";
    }

    if (!diffeomorphicDemonsToolBox::registered())
    {
        qWarning() << "Unable to register diffeomorphicDemons toolbox";
    }

    return true;
}

QString diffeomorphicDemonsPlugin::name() const
{
    return "Diffeomorphic Demons";
}

QStringList diffeomorphicDemonsPlugin::authors() const
{
    QStringList list;
    list << QString::fromUtf8("Benoît Bleuzé");
    return list;
}

QStringList diffeomorphicDemonsPlugin::contributors() const
{
    QStringList list;
    list <<  "Vincent Garcia";
    return list;
}

QString diffeomorphicDemonsPlugin::description() const
{
    QString description;
    description += "<h1>Diffeomorphic Demons Tutorial</h1>";
    description += "Applies the diffeomorphic demons as they can be found in itk.<br>";
    description += "Converts any type of image to float before applying the change, since the diffeomorphic demons only work on float images.<br>";
    description += "See article 'Diffeomorphic Demons Using ITK's Finite Difference Solver Hierarchy' http://www.insight-journal.org/browse/publication/154 for more information about the algorithm.";
    description += "<ul>";
    description += "<li>Drops 2 data with the same size and spacing in the view.</li>";
    description += "<li>Select your parameters.</li>";
    description += "<li>When you are ready, click on 'Run'.</li>";
    description += "<li>The new transformation is added to the transformation list.</li>";
    description += "<li>You can redo, undo, reset, and export the last transformation to a file.</li>";
    description += "</ul>";
    return description;
}

QString diffeomorphicDemonsPlugin::version() const
{
    return DIFFEOMORPHICDEMONSPLUGIN_VERSION;
}

QStringList diffeomorphicDemonsPlugin::types() const
{
    return QStringList() << "diffeomorphicDemons";
}

