/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkDiffusionGradientListData.h>
#include <medItkDiffusionGradientListPlugin.h>

#include <dtkLog/dtkLog.h>

#include <itkLogForwarder.h>

// /////////////////////////////////////////////////////////////////
// medItkDiffusionGradientListPluginPrivate
// /////////////////////////////////////////////////////////////////

class medItkDiffusionGradientListPluginPrivate
{
public:
  medItkDiffusionGradientListPluginPrivate()
  {
      forwarder = itk::LogForwarder::New();
  }
  ~medItkDiffusionGradientListPluginPrivate()
  {

  }
  // Class variables go here.
  itk::LogForwarder::Pointer forwarder;
};

// /////////////////////////////////////////////////////////////////
// medItkDiffusionGradientListPlugin
// /////////////////////////////////////////////////////////////////

medItkDiffusionGradientListPlugin::medItkDiffusionGradientListPlugin(QObject *parent) : dtkPlugin(parent), d(new medItkDiffusionGradientListPluginPrivate)
{

}

medItkDiffusionGradientListPlugin::~medItkDiffusionGradientListPlugin()
{
    delete d;

    d = NULL;
}

bool medItkDiffusionGradientListPlugin::initialize()
{
    if (!medItkDiffusionGradientListData::registered()) {
        dtkWarn() << "Unable to register medItkDiffusionGradientListData type";
    }

    return true;
}

bool medItkDiffusionGradientListPlugin::uninitialize()
{
    return true;
}

QString medItkDiffusionGradientListPlugin::name() const
{
    return "medItkDiffusionGradientListPlugin";
}

QString medItkDiffusionGradientListPlugin::description() const
{
    return tr("List of gradients for Diffusion Weighted Images");
}

QString medItkDiffusionGradientListPlugin::identifier() const
{
    return "medItkDiffusionGradientListPlugin";
}

QString medItkDiffusionGradientListPlugin::version() const
{
    return MEDITKDIFFUSIONGRADIENTLISTPLUGIN_VERSION;
}

QStringList medItkDiffusionGradientListPlugin::tags() const
{
    return QStringList() << "itk" << "data" << "diffusion" << "gradients";
}

QStringList medItkDiffusionGradientListPlugin::types() const
{
    return QStringList() << "medItkDiffusionGradientListData";
}

Q_EXPORT_PLUGIN2(medItkDiffusionGradientListPlugin, medItkDiffusionGradientListPlugin)
