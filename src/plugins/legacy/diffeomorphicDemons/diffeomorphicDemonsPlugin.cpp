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

QString diffeomorphicDemonsPlugin::contact() const
{
    return QString::fromUtf8("benoit.bleuze@inria.fr");
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
    return tr("Applies the diffeomorphic demons as they can be found in itk. Converts any type of image to float before applying the change, since the diffeomorphic demons only work on float images <br/> see: <a href=\"http://www.insight-journal.org/browse/publication/154\" > http://www.insight-journal.org/browse/publication/154 </a>");
}

QString diffeomorphicDemonsPlugin::version() const
{
    return DIFFEOMORPHICDEMONSPLUGIN_VERSION;
}

QStringList diffeomorphicDemonsPlugin::types() const
{
    return QStringList() << "diffeomorphicDemons";
}

