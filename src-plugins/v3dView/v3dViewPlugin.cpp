/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <v3dView.h>
#include <medVtkView.h>
#include <medFiberBundlingToolBox.h>
#include <medViewPropertiesToolBox.h>
#include <medTimeLineToolBox.h>
#include <medTensorViewToolBox.h>
#include <medFiberViewToolBox.h>
#include <medSHViewToolBox.h>
#include <v3dViewMeshInteractor.h>
#include <v3dView4DInteractor.h>
#include <v3dViewAnnotationInteractor.h>
#include "v3dViewPlugin.h"
#ifndef DISABLE_TTK_DEPENDENT_CODE
#include <v3dViewFiberInteractor.h>
#include <v3dViewTensorInteractor.h>
#include <v3dViewSHInteractor.h>
#endif

#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// v3dViewPluginPrivate
// /////////////////////////////////////////////////////////////////

class v3dViewPluginPrivate
{
public:
    // Class variables go here.
};

// /////////////////////////////////////////////////////////////////
// v3dViewPlugin
// /////////////////////////////////////////////////////////////////

v3dViewPlugin::v3dViewPlugin(QObject *parent) : dtkPlugin(parent), d(new v3dViewPluginPrivate)
{

}

v3dViewPlugin::~v3dViewPlugin()
{
    delete d;

    d = NULL;
}

bool v3dViewPlugin::initialize()
{
    if (!medVtkView::registered())                     { dtkWarn() << "Unable to register medVtkView type";              }
    if (!medFiberBundlingToolBox::registered())        { dtkWarn() << "Unable to register medFiberBundlingToolBox type"; }
    if (!medTimeLineToolBox::registered())             { dtkWarn() << "Unable to register medTimeLineToolBox type"; }
    if (!medViewPropertiesToolBox::registered())       { dtkWarn() << "Unable to register medViewPropertiesToolBox type"; }
    if (!medTensorViewToolBox::registered())           { dtkWarn() << "Unable to register medTensorViewToolBox type"; }
    if (!medFiberViewToolBox::registered())            { dtkWarn() << "Unable to register medFiberViewToolBox type"; }
    if (!medSHViewToolBox::registered())               { dtkWarn() << "Unable to register medSHViewToolBox type"; }
    if (!v3dView::registered())                        { dtkWarn() << "Unable to register v3dView type";                     }

#ifndef DISABLE_TTK_DEPENDENT_CODE
    if (!v3dViewFiberInteractor::registered())      { dtkWarn() << "Unable to register v3dViewFiberInteractor type";      }
    if (!v3dViewTensorInteractor::registered())     { dtkWarn() << "Unable to register v3dViewTensorInteractor type";     }
#endif

    if (!v3dViewMeshInteractor::registered())       { dtkWarn() << "Unable to register v3dViewMeshInteractor type";       }
    if (!v3dView4DInteractor::registered())         { dtkWarn() << "Unable to register v3dView4DInteractor type";         }
    if (!v3dViewSHInteractor::registered())         { dtkWarn() << "Unable to register v3dViewSHInteractor type";         }
    if (!v3dViewAnnotationInteractor::registered()) { dtkWarn() << "Unable to register v3dViewAnnotationInteractor type"; }

    return true;
}

bool v3dViewPlugin::uninitialize()
{
    return true;
}

QString v3dViewPlugin::name() const
{
    return "v3dViewPlugin";
}

QString v3dViewPlugin::contact() const
{
    return "";
}

QStringList v3dViewPlugin::authors() const
{
    QStringList list;
    list << QString::fromUtf8("Pierre Fillard");
    return list;
}

QStringList v3dViewPlugin::contributors() const
{
    QStringList list;
    list << "Nicolas Toussaint"
         << "Stephan Schmitt"
         << "John Stark"
         << QString::fromUtf8("Benoît Bleuzé")
         << "Jaime Garcia"
         << "Fatih Arslan"
         << QString::fromUtf8("Théo Papadopoulo")
         << "Julien Wintz"
         << "Sergio Medina";
    return list;
}

QString v3dViewPlugin::version() const
{
    return V3DVIEWPLUGIN_VERSION;
}

QString v3dViewPlugin::description() const
{
    return tr("View Plugin\n<br/>"
              "Contains view and interactors based on vtkinria3d");
}

QStringList v3dViewPlugin::tags() const
{
    return QStringList() << "v3d" << "view";
}

QStringList v3dViewPlugin::types() const
{
    return QStringList() << v3dView::s_identifier() << "v3dViewFiberInteractor"
        << "v3dViewMeshInteractor" << "v3dViewTensorInteractor"
        << "v3dViewSHInteractor"  << "v3dView4DInteractor"
        << v3dViewAnnotationInteractor::s_identifier();
}

Q_EXPORT_PLUGIN2(v3dViewPlugin, v3dViewPlugin)
