/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkDataDiffusionGradientList.h"
#include "itkDataDiffusionGradientListPlugin.h"

#include <dtkLog/dtkLog.h>

#include <dtkItkLogForwarder/itkLogForwarder.h>

// /////////////////////////////////////////////////////////////////
// itkDataDiffusionGradientListPluginPrivate
// /////////////////////////////////////////////////////////////////

class itkDataDiffusionGradientListPluginPrivate
{
public:
  itkDataDiffusionGradientListPluginPrivate()
  {
      forwarder = itk::LogForwarder::New();
  }
  ~itkDataDiffusionGradientListPluginPrivate()
  {

  }
  // Class variables go here.
  itk::LogForwarder::Pointer forwarder;
};

// /////////////////////////////////////////////////////////////////
// itkDataDiffusionGradientListPlugin
// /////////////////////////////////////////////////////////////////

itkDataDiffusionGradientListPlugin::itkDataDiffusionGradientListPlugin(QObject *parent) : dtkPlugin(parent), d(new itkDataDiffusionGradientListPluginPrivate)
{

}

itkDataDiffusionGradientListPlugin::~itkDataDiffusionGradientListPlugin()
{
    delete d;

    d = NULL;
}

bool itkDataDiffusionGradientListPlugin::initialize()
{
    if (!itkDataDiffusionGradientList::registered()) {
        dtkWarn() << "Unable to register itkDataDiffusionGradientList type";
    }

    return true;
}

bool itkDataDiffusionGradientListPlugin::uninitialize()
{
    return true;
}

QString itkDataDiffusionGradientListPlugin::name() const
{
    return "itkDataDiffusionGradientListPlugin";
}

QString itkDataDiffusionGradientListPlugin::description() const
{
    return tr("List of gradients for Diffusion Weighted Images");
}

QString itkDataDiffusionGradientListPlugin::identifier() const
{
    return "itkDataDiffusionGradientListPlugin";
}

QString itkDataDiffusionGradientListPlugin::version() const
{
    return ITKDATADIFFUSIONGRADIENTLISTPLUGIN_VERSION;
}

QStringList itkDataDiffusionGradientListPlugin::tags() const
{
    return QStringList() << "itk" << "data" << "diffusion" << "gradients";
}

QStringList itkDataDiffusionGradientListPlugin::types() const
{
    return QStringList() << "itkDataDiffusionGradientList";
}

Q_EXPORT_PLUGIN2(itkDataDiffusionGradientListPlugin, itkDataDiffusionGradientListPlugin)
