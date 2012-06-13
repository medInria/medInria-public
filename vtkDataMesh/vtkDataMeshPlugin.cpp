#include "vtkDataMeshPlugin.h"
#include "vtkDataMesh.h"
#include "vtkDataMesh4D.h"

#include <dtkLog/dtkLog.h>

#include <dtkVtkLogForwarder/vtkLogForwarder.h>

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

vtkDataMeshPlugin::~vtkDataMeshPlugin(void)
{
    delete d;
    d = NULL;
}

bool vtkDataMeshPlugin::initialize(void)
{
  if(!vtkDataMesh::registered()) qDebug() << "Unable to register vtkDataMesh type";
  if(!vtkDataMesh4D::registered()) qDebug() << "Unable to register vtkDataMesh4D type";
  return true;
}

bool vtkDataMeshPlugin::uninitialize(void)
{
    return true;
}

QString vtkDataMeshPlugin::name(void) const
{
    return "vtkDataMeshPlugin";
}

QString vtkDataMeshPlugin::description(void) const
{
    return "Plugin containing all types of VTK meshes";
}

QString vtkDataMeshPlugin::version(void) const
{
    return VTKDATAMESHPLUGIN_VERSION;
}

QStringList vtkDataMeshPlugin::tags(void) const
{
  return QStringList() << "vtk" << "data" << "mesh" << "4D";
}

QStringList vtkDataMeshPlugin::types(void) const
{
  return QStringList() << "vtkDataMesh";
}

Q_EXPORT_PLUGIN2(vtkDataMeshPlugin, vtkDataMeshPlugin)
