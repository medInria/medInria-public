/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/




#include "v3dView.h"
#include "v3dViewFuseInteractor.h"
#include "v3dViewMeshInteractor.h"
#include "v3dView4DInteractor.h"
#include "v3dViewAnnotationInteractor.h"
#include "v3dViewPlugin.h"
#ifndef DISABLE_TTK_DEPENDENT_CODE
  #include "v3dViewFiberInteractor.h"
  #include "v3dViewTensorInteractor.h"
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
    if (!v3dView::registered())                     { dtkWarn() << "Unable to register v3dView type";                     }

#ifndef DISABLE_TTK_DEPENDENT_CODE
    if (!v3dViewFiberInteractor::registered())      { dtkWarn() << "Unable to register v3dViewFiberInteractor type";      }
#endif

    if (!v3dViewFuseInteractor::registered())       { dtkWarn() << "Unable to register v3dViewFuseInteractor type";       }
    if (!v3dViewMeshInteractor::registered())       { dtkWarn() << "Unable to register v3dViewMeshInteractor type";       }
    if (!v3dView4DInteractor::registered())         { dtkWarn() << "Unable to register v3dView4DInteractor type";         }
    if (!v3dViewAnnotationInteractor::registered()) { dtkWarn() << "Unable to register v3dViewAnnotationInteractor type"; }

#ifndef DISABLE_TTK_DEPENDENT_CODE
    if (!v3dViewTensorInteractor::registered())     { dtkWarn() << "Unable to register v3dViewTensorInteractor type";     }
#endif

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
    list <<  "Nicolas Toussaint"
          << "Stephan Schmitt"
          << "John Stark"
          << QString::fromUtf8("Benoît Bleuzé")
          << "Jaime Garcia"
          << "Fatih Arslan"
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
        << "v3dViewFuseInteractor" << "v3dViewMeshInteractor"
        << "v3dViewTensorInteractor" << "v3dView4DInteractor"
        << v3dViewAnnotationInteractor::s_identifier();
}

Q_EXPORT_PLUGIN2(v3dViewPlugin, v3dViewPlugin)
