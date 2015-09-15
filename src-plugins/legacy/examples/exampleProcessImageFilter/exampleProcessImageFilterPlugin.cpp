/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <exampleProcessImageFilter.h>
#include <exampleProcessImageFilterPlugin.h>
#include <exampleProcessImageFilterToolBox.h> //the toolbox gui and functionality for the process

#include <dtkLog/dtkLog.h>

#include <medToolBoxFactory.h>  //To register the toolbox into the diffusion toolbox

// /////////////////////////////////////////////////////////////////
// exampleProcessImageFilterPluginPrivate
// /////////////////////////////////////////////////////////////////

class exampleProcessImageFilterPluginPrivate
{
public:
    // Class variables go here.
};

// /////////////////////////////////////////////////////////////////
// exampleProcessImageFilterPlugin
// /////////////////////////////////////////////////////////////////

exampleProcessImageFilterPlugin::exampleProcessImageFilterPlugin(QObject *parent) : dtkPlugin(parent), d(new exampleProcessImageFilterPluginPrivate)
{

}

exampleProcessImageFilterPlugin::~exampleProcessImageFilterPlugin()
{
    delete d;

    d = NULL;
}

bool exampleProcessImageFilterPlugin::initialize() {

    //  Register the process

    if (!exampleProcessImageFilter::registered())
        dtkWarn() << "Unable to register exampleProcessImageFilter type";
    else
        dtkWarn() << "ABLE to register exampleProcessImageFilter type";

    //  Register the toolbox of the process with the Custon diffusionToolBox

    if (!medToolBoxFactory::instance()->registerToolBox
            <exampleProcessImageFilterToolBox>("exampleProcessImageFilter", name(), description(), QStringList()<<"diffusion"))
        qDebug() << "Unable to register exampleProcessImageFilter toolbox";
    else
        qDebug() << "ABLE to register exampleProcessImageFilter toolbox";

    return true;
}

bool exampleProcessImageFilterPlugin::uninitialize()
{
    return true;
}

QString exampleProcessImageFilterPlugin::name() const
{
    return "exampleProcessImageFilterPlugin";
}

QString exampleProcessImageFilterPlugin::description() const
{
    return "";
}

QStringList exampleProcessImageFilterPlugin::tags() const
{
    return QStringList();
}

QStringList exampleProcessImageFilterPlugin::types() const
{
    return QStringList() << "exampleProcessImageFilter";
}

Q_EXPORT_PLUGIN2(exampleProcessImageFilterPlugin, exampleProcessImageFilterPlugin)
