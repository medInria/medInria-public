/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <diffeomorphicDemonsProcess.h>
#include <diffeomorphicDemonsPlugin.h>
#include <diffeomorphicDemonsToolBox.h>

diffeomorphicDemonsPlugin::diffeomorphicDemonsPlugin(QObject *parent) : medPlugin(parent)
{
}

bool diffeomorphicDemonsPlugin::initialize()
{
    if (!diffeomorphicDemons::registered())        { dtkWarn() << "Unable to register diffeomorphicDemons type";    }
    if (!diffeomorphicDemonsToolBox::registered()) { dtkWarn() << "Unable to register diffeomorphicDemons toolbox"; }

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
    list << "Vincent Garcia";
    list << medPlugin::contributors();
    return list;
}

QString diffeomorphicDemonsPlugin::description() const
{
    QString description = \
            tr("Applies the diffeomorphic demons algorithm implemented in the <a href=\"https://github.com/Inria-Asclepios/RPI\" style=\"color: #cc0000\" >RPI library</a>. <br><br>\
               Converts any type of image to float before applying the change, since the diffeomorphic demons algorithm only works on float images. \
               <br><br>See: <a href=\"http://www.insight-journal.org/browse/publication/154\" style=\"color: #cc0000\" > http://www.insight-journal.org/browse/publication/154 </a> \
            <br><br>This plugin uses the <a href=\"https://itk.org/\" style=\"color: #cc0000\" >ITK library</a>.");
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

Q_EXPORT_PLUGIN2(diffeomorphicDemonsPlugin, diffeomorphicDemonsPlugin)


