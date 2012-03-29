#include "vtkDataMeshWriterPlugin.h"
#include "vtkDataMeshWriter.h"
#include "vtkDataMesh4DWriter.h"

#include <dtkCore/dtkLog.h>

#include <dtkVtkLogForwarder/vtkLogForwarder.h>

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

vtkDataMeshWriterPlugin::~vtkDataMeshWriterPlugin(void)
{
    delete d;
    d = NULL;
}

bool vtkDataMeshWriterPlugin::initialize(void)
{
  if(!vtkDataMeshWriter::registered()) dtkWarning() << "Unable to register vtkDataMeshWriter type";
  if(!vtkDataMesh4DWriter::registered()) dtkWarning() << "Unable to register vtkDataMeshWriter type";

  return true;
}

bool vtkDataMeshWriterPlugin::uninitialize(void)
{
  return true;
}

QString vtkDataMeshWriterPlugin::name(void) const
{
    return "vtkDataMeshWriterPlugin";
}

QString vtkDataMeshWriterPlugin::description(void) const
{
  return "Writers for any VTK mesh data type.";
}

QString vtkDataMeshWriterPlugin::version(void) const
{
    return VTKDATAMESHWRITERPLUGIN_VERSION;
}

QStringList vtkDataMeshWriterPlugin::tags(void) const
{
    return QStringList() << "vtk" << "data" << "mesh" << "writer";
}

QStringList vtkDataMeshWriterPlugin::types(void) const
{
  return QStringList() << "vtkDataMeshWriter";
}

Q_EXPORT_PLUGIN2(vtkDataMeshWriterPlugin, vtkDataMeshWriterPlugin)
