/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medVtkViewPlugin.h>

#include <medVtkView.h>
#include <medVtkViewNavigator.h>

medVtkViewPlugin::medVtkViewPlugin(QObject *parent) : medPlugin(parent)
{
}

bool medVtkViewPlugin::initialize()
{
    if (!medVtkView::registered()) { dtkWarn() << "Unable to register medVtkView type"; }

    if (!medVtkViewNavigator::registered()) { dtkWarn() << "Unable to register medVtkViewNavigator type"; }

    return true;
}

QString medVtkViewPlugin::name() const
{
    return "VTK View";
}

QStringList medVtkViewPlugin::authors() const
{
    QStringList list;
    list << QString::fromUtf8("rdebroiz");
    return list;
}

QString medVtkViewPlugin::version() const
{
    return MEDVTKVIEWPLUGIN_VERSION;
}

QString medVtkViewPlugin::description() const
{
    return tr("View Plugin\n<br/>"
              "Bring a view based on medVtkinria");
}

QStringList medVtkViewPlugin::tags() const
{
    return QStringList() << "medVtk" << "view";
}

QStringList medVtkViewPlugin::types() const
{
    return QStringList() << medVtkView::s_identifier();
}

//TODO What is it for - RDE
Q_EXPORT_PLUGIN2(medVtkViewPlugin, medVtkViewPlugin)
