#include "vtkDataMeshPlugin.h"
#include "vtkDataMesh.h"

#include "vtkLogForwarder.h"

#include <dtkCore/dtkLog.h>

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

QStringList vtkDataMeshPlugin::tags(void) const
{
  return QStringList() << "vtk" << "data" << "mesh";
}

QStringList vtkDataMeshPlugin::types(void) const
{
  return QStringList() << "vtkDataMesh";
}

Q_EXPORT_PLUGIN2(vtkDataMeshPlugin, vtkDataMeshPlugin)
