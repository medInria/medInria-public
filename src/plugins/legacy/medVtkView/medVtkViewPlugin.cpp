/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medVtkViewPlugin.h>

#include <medVtkView.h>
#include <medVtkViewNavigator.h>

#include <dtkLog/dtkLog.h>

medVtkViewPlugin::medVtkViewPlugin(QObject *parent) :
    medPluginLegacy(parent)
{
}

bool medVtkViewPlugin::initialize()
{
    if (!medVtkView::registered())
    {
        qWarning() << "Unable to register medVtkView type";
    }
    if (!medVtkViewNavigator::registered())
    {
        qWarning() << "Unable to register medVtkViewNavigator type";
    }

    return true;
}

QString medVtkViewPlugin::name() const
{
    return "medVtkViewPlugin";
}

QString medVtkViewPlugin::contact() const
{
    return "medInria team";
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
              "Create views based on medVtkinria");
}

QStringList medVtkViewPlugin::tags() const
{
    return types();
}

QStringList medVtkViewPlugin::types() const
{
    return QStringList() << "medVtkView";
}
