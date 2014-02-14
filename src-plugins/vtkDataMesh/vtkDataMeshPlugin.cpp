/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkDataMeshPlugin.h>
#include <vtkDataMesh.h>
#include <vtkDataMesh4D.h>
#include <vtkDataMeshInteractor.h>

#include <dtkLog/dtkLog.h>

#include <vtkLogForwarder.h>

// /////////////////////////////////////////////////////////////////
// vtkDataMeshPluginPrivate
// /////////////////////////////////////////////////////////////////

class vtkDataMeshPluginPrivate
{
public:
  vtkDataMeshPluginPrivate()
  {
      forwarder = vtkLogForwarder::New();
  }
  ~vtkDataMeshPluginPrivate()
  {
      forwarder->Delete();
  }
  // Class variables go here.
  vtkLogForwarder* forwarder;
};

// /////////////////////////////////////////////////////////////////
// vtkDataMeshPlugin
// /////////////////////////////////////////////////////////////////

vtkDataMeshPlugin::vtkDataMeshPlugin(QObject *parent) : dtkPlugin(parent), d(new vtkDataMeshPluginPrivate)
{

}

vtkDataMeshPlugin::~vtkDataMeshPlugin()
{
    delete d;
    d = NULL;
}

bool vtkDataMeshPlugin::initialize()
{
  if(!vtkDataMesh::registered()) qDebug() << "Unable to register vtkDataMesh type";
  if(!vtkDataMesh4D::registered()) qDebug() << "Unable to register vtkDataMesh4D type";
  if(!vtkDataMeshInteractor::registered()) qDebug() << "Unable to register vtkDataMeshInteractor type";
  return true;
}

bool vtkDataMeshPlugin::uninitialize()
{
    return true;
}

QString vtkDataMeshPlugin::name() const
{
    return "vtkDataMeshPlugin";
}

QString vtkDataMeshPlugin::description() const
{
    return "Plugin containing all types of VTK meshes";
}

QString vtkDataMeshPlugin::version() const
{
    return VTKDATAMESHPLUGIN_VERSION;
}

QStringList vtkDataMeshPlugin::tags() const
{
  return QStringList() << "vtk" << "data" << "mesh" << "4D";
}

QStringList vtkDataMeshPlugin::types() const
{
  return QStringList() << "vtkDataMesh";
}

Q_EXPORT_PLUGIN2(vtkDataMeshPlugin, vtkDataMeshPlugin)
