/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medVtkViewPlugin.h>

#include <medVtkView.h>
#include <medVtkViewNavigator.h>

#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// medVtkViewPluginPrivate
// /////////////////////////////////////////////////////////////////

class medVtkViewPluginPrivate
{
public:
    // Class variables go here.
};

// /////////////////////////////////////////////////////////////////
// medVtkViewPlugin
// /////////////////////////////////////////////////////////////////

medVtkViewPlugin::medVtkViewPlugin(QObject *parent) :
    dtkPlugin(parent), d(new medVtkViewPluginPrivate)
{

}

medVtkViewPlugin::~medVtkViewPlugin()
{
    delete d;
    d = NULL;
}

bool medVtkViewPlugin::initialize()
{
    if (!medVtkView::registered())
        dtkWarn() << "Unable to register medVtkView type";

    if (!medVtkViewNavigator::registered())
        dtkWarn() << "Unable to register medVtkViewNavigator type";

    return true;
}

bool medVtkViewPlugin::uninitialize()
{
    return true;
}

QString medVtkViewPlugin::name() const
{
    return "medVtkViewPlugin";
}

QString medVtkViewPlugin::contact() const
{
    return "medinria team";
}

QStringList medVtkViewPlugin::authors() const
{
    QStringList list;
    list << QString::fromUtf8("rdebroiz");
    return list;
}

QStringList medVtkViewPlugin::contributors() const
{
    QStringList list;
    list << "rdebroiz";

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
