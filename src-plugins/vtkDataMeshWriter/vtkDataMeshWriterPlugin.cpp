/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkDataMeshWriterPlugin.h"
#include "vtkDataMeshWriter.h"
#include "vtkDataMesh4DWriter.h"

#include <dtkLog/dtkLog.h>

#include <vtkLogForwarder.h>

// /////////////////////////////////////////////////////////////////
// vtkDataMeshWriterPluginPrivate
// /////////////////////////////////////////////////////////////////

class vtkDataMeshWriterPluginPrivate
{
public:
  vtkDataMeshWriterPluginPrivate()
  {
      forwarder = vtkLogForwarder::New();
  }
  ~vtkDataMeshWriterPluginPrivate()
  {
      forwarder->Delete();
  }
  // Class variables go here.
  vtkLogForwarder* forwarder;
};

// /////////////////////////////////////////////////////////////////
// vtkDataMeshWriterPlugin
// /////////////////////////////////////////////////////////////////

vtkDataMeshWriterPlugin::vtkDataMeshWriterPlugin(QObject *parent) : dtkPlugin(parent), d(new vtkDataMeshWriterPluginPrivate)
{
}

vtkDataMeshWriterPlugin::~vtkDataMeshWriterPlugin()
{
    delete d;
    d = NULL;
}

bool vtkDataMeshWriterPlugin::initialize()
{
    if (!vtkDataMeshWriter::registered())   { dtkWarn() << "Unable to register vtkDataMeshWriter type"; }
    if (!vtkDataMesh4DWriter::registered()) { dtkWarn() << "Unable to register vtkDataMeshWriter type"; }
    
    return true;
}

bool vtkDataMeshWriterPlugin::uninitialize()
{
  return true;
}

QString vtkDataMeshWriterPlugin::name() const
{
    return "vtkDataMeshWriterPlugin";
}

QString vtkDataMeshWriterPlugin::description() const
{
  return "Writers for any VTK mesh data type.";
}

QString vtkDataMeshWriterPlugin::version() const
{
    return VTKDATAMESHWRITERPLUGIN_VERSION;
}

QStringList vtkDataMeshWriterPlugin::tags() const
{
    return QStringList() << "vtk" << "data" << "mesh" << "writer";
}

QStringList vtkDataMeshWriterPlugin::types() const
{
  return QStringList() << "vtkDataMeshWriter";
}

Q_EXPORT_PLUGIN2(vtkDataMeshWriterPlugin, vtkDataMeshWriterPlugin)
