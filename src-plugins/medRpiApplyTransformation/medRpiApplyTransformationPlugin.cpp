/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medRpiApplyTransformationProcess.h>
#include <medRpiApplyTransformationPlugin.h>

#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// medRpiApplyTransformationPluginPrivate
// /////////////////////////////////////////////////////////////////

class medRpiApplyTransformationPluginPrivate
{
public:
    // Class variables go here.
};

// /////////////////////////////////////////////////////////////////
// medRpiApplyTransformationPlugin
// /////////////////////////////////////////////////////////////////

medRpiApplyTransformationPlugin::medRpiApplyTransformationPlugin(QObject *parent) : dtkPlugin(parent), d(new medRpiApplyTransformationPluginPrivate)
{

}

medRpiApplyTransformationPlugin::~medRpiApplyTransformationPlugin()
{
    delete d;
    d = NULL;
}

bool medRpiApplyTransformationPlugin::initialize()
{
    if (!medRpiApplyTransformationProcess::registered()) { dtkWarn() << "Unable to register medRpiApplyTransformationProcess type";    }

    return true;
}

bool medRpiApplyTransformationPlugin::uninitialize()
{
    return true;
}

QString medRpiApplyTransformationPlugin::name() const
{
    return "medRpiApplyTransformation";
}

QString medRpiApplyTransformationPlugin::contact() const
{
    return QString::fromUtf8("rene-paul.debroize@inria.fr");
}

QStringList medRpiApplyTransformationPlugin::authors() const
{
    QStringList list;
    list << QString::fromUtf8("René - Paul Debroize");
    return list;
}

QStringList medRpiApplyTransformationPlugin::contributors() const
{
    QStringList list;
    return list;
}

QString medRpiApplyTransformationPlugin::description() const
{
    return tr("Applies linear, SVF or DF transformations to a scalar image");
}


QString medRpiApplyTransformationPlugin::version() const
{
    return MEDDIFFEOMORPHICDEMONSREGISTRATIONPLUGIN_VERSION;
}

QStringList medRpiApplyTransformationPlugin::dependencies() const
{
    return QStringList();
}


QStringList medRpiApplyTransformationPlugin::tags() const
{
    return QStringList();
}


QStringList medRpiApplyTransformationPlugin::types() const
{
    return QStringList() << "medRpiApplyTransformationProcess";
}

Q_EXPORT_PLUGIN2(medRpiApplyTransformationPlugin, medRpiApplyTransformationPlugin)


